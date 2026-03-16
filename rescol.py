import typer
import subprocess
import os
import re
import time
from pathlib import Path
from typing import Optional, Annotated, List, Dict, Any
from rich.console import Console
from rich.panel import Panel
from rich.table import Table
from rich.progress import Progress, SpinnerColumn, TextColumn
from rich.prompt import Prompt, IntPrompt, FloatPrompt
from benchmark_manager import BenchmarkManager, LegacyMySQLSync

app = typer.Typer(help="RESCOL: Ant Colony Optimization for Waste Collection")
preset_app = typer.Typer(help="Manage configuration presets")
app.add_typer(preset_app, name="preset")

console = Console()
db = BenchmarkManager()

def extract_metrics(output: str) -> Dict[str, float]:
    """Extract metrics from RESCOL output using regex."""
    metrics = {}
    cost_match = re.search(r"Mejor costo:\s*([\d\.]+)", output)
    if cost_match:
        metrics["cost"] = float(cost_match.group(1))
    
    length_match = re.search(r"Mejor longitud:\s*([\d\.]+)", output)
    if length_match:
        metrics["length"] = float(length_match.group(1))
        
    return metrics

@app.command()
def interactive():
    """
    Step-by-step interactive configuration for RESCOL.
    """
    console.print(Panel("[bold cyan]Welcome to RESCOL Interactive Mode[/]\n[italic]Guided configuration for ACO algorithms[/]", border_style="cyan"))

    # 1. Select Instance
    instancias_dir = Path("Instancias")
    if not instancias_dir.exists():
        console.print("[bold red]Error:[/] Instancias/ directory not found.")
        raise typer.Exit(code=1)
    
    files = sorted([f.name for f in instancias_dir.glob("*.txt")])
    if not files:
        console.print("[bold yellow]Warning:[/] No .txt files found in Instancias/.")
        instance_str = Prompt.ask("Enter the path to your instance file")
    else:
        table = Table(title="Available Instances", show_header=True)
        table.add_column("#", style="dim")
        table.add_column("Filename", style="green")
        for i, f in enumerate(files):
            table.add_row(str(i+1), f)
        console.print(table)
        
        idx = IntPrompt.ask("Select an instance number", default=1)
        if 1 <= idx <= len(files):
            instance_str = files[idx-1]
        else:
            instance_str = files[0]

    # 2. Select Method
    methods = ["AntSystem", "MinMax", "Elitist", "ACS"]
    table_m = Table(title="ACO Methods", show_header=True)
    table_m.add_column("ID", style="dim")
    table_m.add_column("Method", style="magenta")
    for i, m in enumerate(methods):
        table_m.add_row(str(i), m)
    console.print(table_m)
    method_id = IntPrompt.ask("Select method ID", choices=["0", "1", "2", "3"], default=1)

    # 3. Basic Parameters
    ants = IntPrompt.ask("Number of ants", default=10)
    epochs = IntPrompt.ask("Number of epochs", default=1)
    
    use_eval = Prompt.ask("Termination criterion", choices=["evals", "iters"], default="evals")
    if use_eval == "evals":
        evaluations = IntPrompt.ask("Max evaluations", default=500)
        iterations = None
    else:
        iterations = IntPrompt.ask("Max iterations", default=100)
        evaluations = None

    # 4. Hyperparameters
    alpha = FloatPrompt.ask("Alpha", default=1.0)
    beta = FloatPrompt.ask("Beta", default=2.0)
    rho = FloatPrompt.ask("Rho", default=0.5)

    # 5. Advanced
    use_lim = Prompt.ask("Use pass limiter?", choices=["y", "n"], default="n")
    limiter = None
    if use_lim == "y":
        limiter = IntPrompt.ask("Max passes per edge", default=2)

    console.print("\n[bold green]Ready to launch![/]")
    
    run(
        instance=Path(instance_str),
        method=int(method_id),
        ants=ants,
        iterations=iterations,
        evaluations=evaluations,
        epochs=epochs,
        alpha=alpha,
        beta=beta,
        rho=rho,
        limiter=limiter,
        verbose=False
    )

@app.command()
def run(
    instance: Annotated[Path, typer.Argument(help="Path to the instance file")] ,
    method: Annotated[int, typer.Option("--method", "-m", help="ACO Method: 0=AS, 1=MM, 3=ACS")] = 1,
    ants: Annotated[int, typer.Option("--ants", "-n", help="Number of ants")] = 10,
    iterations: Annotated[Optional[int], typer.Option("--iter", "-i", help="Max iterations")] = None,
    evaluations: Annotated[Optional[int], typer.Option("--eval", "-e", help="Max evaluations")] = None,
    epochs: Annotated[int, typer.Option("--epochs", help="Number of epochs")] = 1,
    alpha: Annotated[float, typer.Option("--alpha", "-a", help="Alpha parameter")] = 1.0,
    beta: Annotated[float, typer.Option("--beta", "-b", help="Beta parameter")] = 2.0,
    rho: Annotated[float, typer.Option("--rho", help="Pheromone evaporation rate")] = 0.5,
    limiter: Annotated[Optional[int], typer.Option("--limit", help="Max passes per edge")] = None,
    seed: Annotated[Optional[int], typer.Option("--seed", help="Random seed")] = None,
    binary_path: Annotated[Path, typer.Option("--bin", help="Path to RESCOL binary")] = Path("./build/RESCOL"),
    verbose: Annotated[bool, typer.Option("--verbose", "-v", help="Show all output")] = False,
    use_db: Annotated[bool, typer.Option("--db", help="Use legacy MySQL database")] = False,
    quiet: bool = False,
) -> Optional[Dict[str, Any]]:
    """
    Run the RESCOL ACO algorithm.
    """
    if not binary_path.exists():
        console.print(f"[bold red]Error:[/] Binary not found at {binary_path}.")
        return None

    if not instance.exists():
        alt_path = Path("Instancias") / instance
        if alt_path.exists():
            instance = alt_path
        else:
            console.print(f"[bold red]Error:[/] Instance file not found: {instance}")
            return None

    cmd = [
        str(binary_path),
        str(instance),
        "--metodo", str(method),
        "--num-hormigas", str(ants),
        "--epocas", str(epochs),
        "--alfa", str(alpha),
        "--beta", str(beta),
        "--rho", str(rho),
        "--irace"
    ]

    if evaluations:
        cmd += ["--eval-max", str(evaluations), "--usar-evaluaciones"]
    elif iterations:
        cmd += ["--iter-max", str(iterations), "--usar-iteraciones"]
    else:
        cmd += ["--eval-max", "100", "--usar-evaluaciones"]

    if limiter is not None:
        cmd += ["--usar-limitador", "--valor-limitador", str(limiter)]

    if use_db:
        cmd += ["--bdconn"]

    if seed is not None:
        cmd += ["--semilla", str(seed)]
    else:
        cmd += ["--random-seed"]

    if not verbose:
        cmd += ["--silence"]

    if not quiet:
        table = Table(title="RESCOL Configuration", show_header=False, box=None)
        table.add_row("Instance", str(instance))
        table.add_row("Method", ["AntSystem", "MinMax", "Elitist", "ACS"][method] if method < 4 else "Other")
        table.add_row("Ants", str(ants))
        console.print(Panel(table, border_style="cyan", title="[bold]Launch Parameters[/]"))

    start_time = time.time()
    result = subprocess.run(cmd, capture_output=True, text=True)
    end_time = time.time()
    
    if result.returncode == 0:
        metrics = extract_metrics(result.stdout)
        metrics["duration_ms"] = (end_time - start_time) * 1000
        
        if not quiet:
            console.print("[bold green]✔[/] Execution completed!")
            summary = result.stdout.split("Mejor resultado")[-1].strip() if "Mejor resultado" in result.stdout else result.stdout
            console.print(Panel(summary, title="[bold green]Results[/]", border_style="green"))
        return metrics
    else:
        if not quiet:
            console.print("[bold red]✘[/] Execution failed!")
            console.print(result.stderr)
        return None

@app.command()
def benchmark(
    tag: Annotated[str, typer.Option("--tag", "-t", help="Tag for this run")] = "manual-bench",
    instances: Annotated[str, typer.Option("--instances", help="Glob pattern")] = "Formato5x5*.txt",
    preset: Annotated[Optional[str], typer.Option("--preset", "-p", help="Preset to use")] = None,
    method: int = 1,
    ants: int = 10,
    evals: int = 500,
):
    """Run benchmark over multiple instances."""
    params = {"method": method, "ants": ants, "evaluations": evals, "epochs": 1}
    preset_id = None
    if preset:
        p_data = db.get_preset(preset)
        if p_data:
            params = p_data
            preset_id = p_data["id"]

    instancias_dir = Path("Instancias")
    files = list(instancias_dir.glob(instances))
    if not files:
        console.print(f"[bold red]Error:[/] No files matching '{instances}' found.")
        return

    run_id = db.create_run(tag)
    
    with Progress(SpinnerColumn(), TextColumn("[progress.description]{task.description}")) as progress:
        task = progress.add_task(description="Benchmarking...", total=len(files))
        for f in sorted(files):
            progress.update(task, description=f"Running {f.name}...")
            res = run(instance=f, quiet=True, **params)
            if res:
                db.save_result(run_id, preset_id, f.name, params["method"], res["cost"], res["length"], res["duration_ms"])
            progress.advance(task)

    report(tag)

@app.command()
def report(tag: str):
    """Generate result report for a tag."""
    results = db.get_results_by_tag(tag)
    if not results:
        console.print(f"[bold red]No data for tag:[/] {tag}")
        return

    table = Table(title=f"Report: {tag}")
    table.add_column("Instance", style="green")
    table.add_column("Preset", style="cyan")
    table.add_column("Cost", justify="right", style="bold yellow")
    table.add_column("Length", justify="right")
    table.add_column("Time (ms)", justify="right")

    for r in results:
        table.add_row(r["instance"], r["preset_name"] or "-", f"{r['cost']:.2f}", str(int(r['length'])), f"{r['duration_ms']:.1f}")
    
    console.print(table)

@app.command()
def sync(tag: str, config_ini: str = "config.ini"):
    """Sync results for a tag to the legacy MySQL database."""
    results = db.get_results_by_tag(tag)
    if not results:
        console.print(f"[bold red]No results to sync for tag:[/] {tag}")
        return

    syncer = LegacyMySQLSync(config_ini)
    with Progress(SpinnerColumn(), TextColumn("[progress.description]{task.description}")) as progress:
        task = progress.add_task(description=f"Syncing {len(results)} results...", total=len(results))
        success_count = 0
        for r in results:
            # Convert ms to us for legacy schema
            if syncer.sync_result(r["instance"], r["method"], r["cost"], r["length"], int(r["duration_ms"] * 1000)):
                success_count += 1
            progress.advance(task)
    
    if success_count > 0:
        console.print(f"[bold green]✔[/] Synced {success_count} results to legacy DB.")
    else:
        console.print("[bold red]✘[/] All syncs failed. Check your config.ini and connection.")

@preset_app.command("add")
def preset_add(name: str, method: int = 1, ants: int = 10, evals: int = 500):
    """Save a new preset."""
    db.save_preset(name, {"method": method, "ants": ants, "evaluations": evals})
    console.print(f"[bold green]✔[/] Preset '{name}' saved.")

@preset_app.command("list")
def preset_list():
    """List all presets."""
    presets = db.list_presets()
    table = Table(title="Presets")
    table.add_column("Name", style="cyan")
    table.add_column("Met", style="magenta")
    table.add_column("Ants", justify="right")
    for p in presets:
        table.add_row(p["name"], str(p["method"]), str(p["ants"]))
    console.print(table)

@app.command()
def build(clean: bool = False):
    """Build project."""
    if clean and os.path.exists("build"):
        import shutil
        shutil.rmtree("build")
    os.makedirs("build", exist_ok=True)
    subprocess.run(["cmake", ".."], cwd="build", check=True)
    subprocess.run(["make", "-j4"], cwd="build", check=True)

if __name__ == "__main__":
    app()
