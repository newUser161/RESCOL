\ Model ARP
\ LP format - for model browsing. Use MPS format to capture full model detail.
Minimize
  8 x[0,1] + 3 x[1,21] + 4 x[22,3] + 9 x[21,61] + x[62,22] + 23 x[67,101]
   + 21 x[102,68] + 3 x[141,13] + 4 x[15,142] + 4 x[107,141] + 5 x[142,108]
   + 8 x[41,0] + 9 x[81,42] + 8 x[12,82] + 9 x[1,53] + 8 x[54,93]
   + 19 x[94,13] + 21 x[3,71] + 9 x[72,111] + 5 x[112,15] + 7 x[13,12]
   + 9 x[63,55] + 23 x[56,65] + 12 x[71,64] + 9 x[66,72] + 34 x[103,95]
   + 12 x[96,105] + 23 x[111,104] + 12 x[106,112] + 4 x[51,41] + x[42,52]
   + 4 x[91,81] + x[82,92]
Subject To
 VisitarTodosArcos[0,1]: x[0,1] >= 1
 VisitarTodosArcos[1,21]: x[1,21] >= 1
 VisitarTodosArcos[22,3]: x[22,3] >= 1
 VisitarTodosArcos[21,61]: x[21,61] >= 1
 VisitarTodosArcos[62,22]: x[62,22] >= 1
 VisitarTodosArcos[67,101]: x[67,101] >= 1
 VisitarTodosArcos[102,68]: x[102,68] >= 1
 VisitarTodosArcos[141,13]: x[141,13] >= 1
 VisitarTodosArcos[15,142]: x[15,142] >= 1
 VisitarTodosArcos[107,141]: x[107,141] >= 1
 VisitarTodosArcos[142,108]: x[142,108] >= 1
 VisitarTodosArcos[41,0]: x[41,0] >= 1
 VisitarTodosArcos[81,42]: x[81,42] >= 1
 VisitarTodosArcos[12,82]: x[12,82] >= 1
 VisitarTodosArcos[1,53]: x[1,53] >= 1
 VisitarTodosArcos[54,93]: x[54,93] >= 1
 VisitarTodosArcos[94,13]: x[94,13] >= 1
 VisitarTodosArcos[3,71]: x[3,71] >= 1
 VisitarTodosArcos[72,111]: x[72,111] >= 1
 VisitarTodosArcos[112,15]: x[112,15] >= 1
 VisitarTodosArcos[13,12]: x[13,12] >= 1
 VisitarTodosArcosBidireccionales[63,55,56,65]: x[63,55] + x[56,65] >= 1
 VisitarTodosArcosBidireccionales[71,64,66,72]: x[71,64] + x[66,72] >= 1
 VisitarTodosArcosBidireccionales[103,95,96,105]: x[103,95] + x[96,105]
   >= 1
 VisitarTodosArcosBidireccionales[111,104,106,112]: x[111,104] + x[106,112]
   >= 1
 VisitarTodosArcosBidireccionales[51,41,42,52]: x[51,41] + x[42,52] >= 1
 VisitarTodosArcosBidireccionales[91,81,82,92]: x[91,81] + x[82,92] >= 1
 ConservacionDeFlujoPuntas[0,0]: - x[0,1] + x[41,0] + s_i[0] - t_i[0] = 0
 ConservacionDeFlujoPuntas[1,1]: x[0,1] - x[1,21] - x[1,53] + s_i[1]
   - t_i[1] = 0
 ConservacionDeFlujoPuntas[3,3]: x[22,3] - x[3,71] + s_i[3] - t_i[3] = 0
 ConservacionDeFlujoPuntas[12,12]: - x[12,82] + x[13,12] + s_i[12]
   - t_i[12] = 0
 ConservacionDeFlujoPuntas[141,141]: - x[141,13] + x[107,141] + s_i[141]
   - t_i[141] = 0
 ConservacionDeFlujoPuntas[13,13]: x[141,13] + x[94,13] - x[13,12]
   + s_i[13] - t_i[13] = 0
 ConservacionDeFlujoPuntas[15,15]: - x[15,142] + x[112,15] + s_i[15]
   - t_i[15] = 0
 ConservacionDeFlujoPuntas[142,142]: x[15,142] - x[142,108] + s_i[142]
   - t_i[142] = 0
 ConservacionDeFlujoPuntas[21,21]: x[1,21] - x[21,61] + s_i[21] - t_i[21]
   = 0
 ConservacionDeFlujoPuntas[22,22]: - x[22,3] + x[62,22] + s_i[22] - t_i[22]
   = 0
 ConservacionDeFlujoPuntas[41,41]: - x[41,0] + x[51,41] + s_i[41] - t_i[41]
   = 0
 ConservacionDeFlujoPuntas[42,42]: x[81,42] - x[42,52] + s_i[42] - t_i[42]
   = 0
 ConservacionDeFlujoPuntas[51,51]: - x[51,41] + s_i[51] - t_i[51] = 0
 ConservacionDeFlujoPuntas[52,52]: x[42,52] + s_i[52] - t_i[52] = 0
 ConservacionDeFlujoPuntas[53,53]: x[1,53] + s_i[53] - t_i[53] = 0
 ConservacionDeFlujoPuntas[54,54]: - x[54,93] + s_i[54] - t_i[54] = 0
 ConservacionDeFlujoPuntas[55,55]: x[63,55] + s_i[55] - t_i[55] = 0
 ConservacionDeFlujoPuntas[56,56]: - x[56,65] + s_i[56] - t_i[56] = 0
 ConservacionDeFlujoPuntas[61,61]: x[21,61] + s_i[61] - t_i[61] = 0
 ConservacionDeFlujoPuntas[62,62]: - x[62,22] + s_i[62] - t_i[62] = 0
 ConservacionDeFlujoPuntas[63,63]: - x[63,55] + s_i[63] - t_i[63] = 0
 ConservacionDeFlujoPuntas[64,64]: x[71,64] + s_i[64] - t_i[64] = 0
 ConservacionDeFlujoPuntas[65,65]: x[56,65] + s_i[65] - t_i[65] = 0
 ConservacionDeFlujoPuntas[66,66]: - x[66,72] + s_i[66] - t_i[66] = 0
 ConservacionDeFlujoPuntas[67,67]: - x[67,101] + s_i[67] - t_i[67] = 0
 ConservacionDeFlujoPuntas[68,68]: x[102,68] + s_i[68] - t_i[68] = 0
 ConservacionDeFlujoPuntas[71,71]: x[3,71] - x[71,64] + s_i[71] - t_i[71]
   = 0
 ConservacionDeFlujoPuntas[72,72]: - x[72,111] + x[66,72] + s_i[72]
   - t_i[72] = 0
 ConservacionDeFlujoPuntas[81,81]: - x[81,42] + x[91,81] + s_i[81]
   - t_i[81] = 0
 ConservacionDeFlujoPuntas[82,82]: x[12,82] - x[82,92] + s_i[82] - t_i[82]
   = 0
 ConservacionDeFlujoPuntas[91,91]: - x[91,81] + s_i[91] - t_i[91] = 0
 ConservacionDeFlujoPuntas[92,92]: x[82,92] + s_i[92] - t_i[92] = 0
 ConservacionDeFlujoPuntas[93,93]: x[54,93] + s_i[93] - t_i[93] = 0
 ConservacionDeFlujoPuntas[94,94]: - x[94,13] + s_i[94] - t_i[94] = 0
 ConservacionDeFlujoPuntas[95,95]: x[103,95] + s_i[95] - t_i[95] = 0
 ConservacionDeFlujoPuntas[96,96]: - x[96,105] + s_i[96] - t_i[96] = 0
 ConservacionDeFlujoPuntas[101,101]: x[67,101] + s_i[101] - t_i[101] = 0
 ConservacionDeFlujoPuntas[102,102]: - x[102,68] + s_i[102] - t_i[102] = 0
 ConservacionDeFlujoPuntas[103,103]: - x[103,95] + s_i[103] - t_i[103] = 0
 ConservacionDeFlujoPuntas[104,104]: x[111,104] + s_i[104] - t_i[104] = 0
 ConservacionDeFlujoPuntas[105,105]: x[96,105] + s_i[105] - t_i[105] = 0
 ConservacionDeFlujoPuntas[106,106]: - x[106,112] + s_i[106] - t_i[106]
   = 0
 ConservacionDeFlujoPuntas[107,107]: - x[107,141] + s_i[107] - t_i[107]
   = 0
 ConservacionDeFlujoPuntas[108,108]: x[142,108] + s_i[108] - t_i[108] = 0
 ConservacionDeFlujoPuntas[111,111]: x[72,111] - x[111,104] + s_i[111]
   - t_i[111] = 0
 ConservacionDeFlujoPuntas[112,112]: - x[112,15] + x[106,112] + s_i[112]
   - t_i[112] = 0
 NodoInicial: s_i[0] + s_i[1] + s_i[3] + s_i[12] + s_i[141] + s_i[13]
   + s_i[15] + s_i[142] + s_i[21] + s_i[22] + s_i[41] + s_i[42] + s_i[51]
   + s_i[52] + s_i[53] + s_i[54] + s_i[55] + s_i[56] + s_i[61] + s_i[62]
   + s_i[63] + s_i[64] + s_i[65] + s_i[66] + s_i[67] + s_i[68] + s_i[71]
   + s_i[72] + s_i[81] + s_i[82] + s_i[91] + s_i[92] + s_i[93] + s_i[94]
   + s_i[95] + s_i[96] + s_i[101] + s_i[102] + s_i[103] + s_i[104]
   + s_i[105] + s_i[106] + s_i[107] + s_i[108] + s_i[111] + s_i[112] = 1
 NodoTerminal: t_i[0] + t_i[1] + t_i[3] + t_i[12] + t_i[141] + t_i[13]
   + t_i[15] + t_i[142] + t_i[21] + t_i[22] + t_i[41] + t_i[42] + t_i[51]
   + t_i[52] + t_i[53] + t_i[54] + t_i[55] + t_i[56] + t_i[61] + t_i[62]
   + t_i[63] + t_i[64] + t_i[65] + t_i[66] + t_i[67] + t_i[68] + t_i[71]
   + t_i[72] + t_i[81] + t_i[82] + t_i[91] + t_i[92] + t_i[93] + t_i[94]
   + t_i[95] + t_i[96] + t_i[101] + t_i[102] + t_i[103] + t_i[104]
   + t_i[105] + t_i[106] + t_i[107] + t_i[108] + t_i[111] + t_i[112] = 1
Bounds
Binaries
 s_i[0] s_i[1] s_i[3] s_i[12] s_i[141] s_i[13] s_i[15] s_i[142] s_i[21]
 s_i[22] s_i[41] s_i[42] s_i[51] s_i[52] s_i[53] s_i[54] s_i[55] s_i[56]
 s_i[61] s_i[62] s_i[63] s_i[64] s_i[65] s_i[66] s_i[67] s_i[68] s_i[71]
 s_i[72] s_i[81] s_i[82] s_i[91] s_i[92] s_i[93] s_i[94] s_i[95] s_i[96]
 s_i[101] s_i[102] s_i[103] s_i[104] s_i[105] s_i[106] s_i[107] s_i[108]
 s_i[111] s_i[112] t_i[0] t_i[1] t_i[3] t_i[12] t_i[141] t_i[13] t_i[15]
 t_i[142] t_i[21] t_i[22] t_i[41] t_i[42] t_i[51] t_i[52] t_i[53] t_i[54]
 t_i[55] t_i[56] t_i[61] t_i[62] t_i[63] t_i[64] t_i[65] t_i[66] t_i[67]
 t_i[68] t_i[71] t_i[72] t_i[81] t_i[82] t_i[91] t_i[92] t_i[93] t_i[94]
 t_i[95] t_i[96] t_i[101] t_i[102] t_i[103] t_i[104] t_i[105] t_i[106]
 t_i[107] t_i[108] t_i[111] t_i[112]
Generals
 x[0,1] x[1,21] x[22,3] x[21,61] x[62,22] x[67,101] x[102,68] x[141,13]
 x[15,142] x[107,141] x[142,108] x[41,0] x[81,42] x[12,82] x[1,53] x[54,93]
 x[94,13] x[3,71] x[72,111] x[112,15] x[13,12] x[63,55] x[56,65] x[71,64]
 x[66,72] x[103,95] x[96,105] x[111,104] x[106,112] x[51,41] x[42,52]
 x[91,81] x[82,92]
End
