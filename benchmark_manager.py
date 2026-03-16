import sqlite3
import json
from pathlib import Path
from datetime import datetime
from typing import List, Dict, Any, Optional

DB_PATH = Path("benchmarks.db")

class BenchmarkManager:
    def __init__(self):
        self.conn = sqlite3.connect(DB_PATH)
        self._init_db()

    def _init_db(self):
        cursor = self.conn.cursor()
        
        # Table for configuration presets
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS presets (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT UNIQUE,
                method INTEGER,
                alpha REAL,
                beta REAL,
                rho REAL,
                ants INTEGER,
                evaluations INTEGER,
                iterations INTEGER,
                epochs INTEGER,
                limiter INTEGER,
                description TEXT
            )
        ''')

        # Table for benchmark runs (groups of results with a tag)
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS runs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                tag TEXT,
                timestamp DATETIME,
                notes TEXT
            )
        ''')

        # Table for individual results
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS results (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                run_id INTEGER,
                preset_id INTEGER,
                instance TEXT,
                method INTEGER,
                cost REAL,
                length REAL,
                duration_ms REAL,
                FOREIGN KEY(run_id) REFERENCES runs(id),
                FOREIGN KEY(preset_id) REFERENCES presets(id)
            )
        ''')
        
        self.conn.commit()

    def save_preset(self, name: str, params: Dict[str, Any]):
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT OR REPLACE INTO presets (
                name, method, alpha, beta, rho, ants, evaluations, iterations, epochs, limiter
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
            name, 
            params.get("method", 1),
            params.get("alpha", 1.0),
            params.get("beta", 2.0),
            params.get("rho", 0.5),
            params.get("ants", 10),
            params.get("evaluations"),
            params.get("iterations"),
            params.get("epochs", 1),
            params.get("limiter")
        ))
        self.conn.commit()

    def get_preset(self, name: str) -> Optional[Dict[str, Any]]:
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM presets WHERE name = ?', (name,))
        row = cursor.fetchone()
        if not row:
            return None
        
        # Map columns to dict
        cols = ["id", "name", "method", "alpha", "beta", "rho", "ants", "evaluations", "iterations", "epochs", "limiter", "description"]
        return dict(zip(cols, row))

    def list_presets(self) -> List[Dict[str, Any]]:
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM presets')
        cols = ["id", "name", "method", "alpha", "beta", "rho", "ants", "evaluations", "iterations", "epochs", "limiter", "description"]
        return [dict(zip(cols, row)) for row in cursor.fetchall()]

    def create_run(self, tag: str, notes: Optional[str] = None) -> int:
        cursor = self.conn.cursor()
        cursor.execute('INSERT INTO runs (tag, timestamp, notes) VALUES (?, ?, ?)', 
                       (tag, datetime.now().isoformat(), notes))
        self.conn.commit()
        return cursor.lastrowid

    def save_result(self, run_id: int, preset_id: Optional[int], instance: str, method: int, cost: float, length: float, duration_ms: float):
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT INTO results (run_id, preset_id, instance, method, cost, length, duration_ms)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (run_id, preset_id, instance, method, cost, length, duration_ms))
        self.conn.commit()

    def get_results_by_tag(self, tag: str) -> List[Dict[str, Any]]:
        cursor = self.conn.cursor()
        cursor.execute('''
            SELECT res.*, r.tag, r.timestamp, p.name as preset_name
            FROM results res
            JOIN runs r ON res.run_id = r.id
            LEFT JOIN presets p ON res.preset_id = p.id
            WHERE r.tag = ?
            ORDER BY res.instance, res.cost ASC
        ''', (tag,))
        rows = cursor.fetchall()
        # Note: results table has 8 cols + 3 joined cols = 11
        cols = ["id", "run_id", "preset_id", "instance", "method", "cost", "length", "duration_ms", "tag", "timestamp", "preset_name"]
        return [dict(zip(cols, row)) for row in rows]

    def close(self):
        self.conn.close()

class LegacyMySQLSync:
    """Helper to sync results to the project's original MySQL schema."""
    def __init__(self, config_path: str = "config.ini"):
        self.config_path = config_path
        self.conn = None

    def _connect(self):
        import mysql.connector
        import configparser
        config = configparser.ConfigParser()
        if not os.path.exists(self.config_path):
            return None
        
        config.read(self.config_path)
        try:
            self.conn = mysql.connector.connect(
                host=config['database']['host'],
                user=config['database']['user'],
                password=config['database']['password'],
                database=config['database']['database']
            )
            return self.conn
        except Exception as e:
            print(f"Connection failed: {e}")
            return None

    def sync_result(self, instance: str, method: int, cost: float, length: float, time_us: int):
        conn = self._connect()
        if not conn:
            return False
            
        # This is a simplified version of POSTDB.py logic
        # For a full sync, we'd need all ACOArgs which are currently not fully stored in results
        # but we can try to push the experiment metrics.
        cursor = conn.cursor()
        query = '''INSERT INTO experimentos_ACO_final (nombre_instancia, metodo, costo_ruta, longitud_ruta, tiempo_res_us) 
                   VALUES (%s, %s, %s, %s, %s)'''
        cursor.execute(query, (instance, method, cost, length, time_us))
        conn.commit()
        cursor.close()
        conn.close()
        return True
