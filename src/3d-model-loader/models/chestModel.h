#ifndef CHEST_MODEL_H
#define CHEST_MODEL_H

#define CHEST_VERTEX_COUNT 118
const S3L_Unit chestVertices[CHEST_VERTEX_COUNT * 3] = {
   1335, -1581,   959,        // 0
   1504,  -822,  1122,        // 3
   1504,  -822,  -971,        // 6
   1335, -1581,  -808,        // 9
   1656,  -822,  1273,        // 12
   1656,  -822, -1123,        // 15
   1451,   823,   707,        // 18
   1484,   549,   532,        // 21
   1539,    66,   860,        // 24
   1521,   214,  1160,        // 27
    331,  -501,  -969,        // 30
   1247,  -501,  -969,        // 33
   1163,   101,  -875,        // 36
    287,    95,  -875,        // 39
    223,   647,  -479,        // 42
    250,   794,  -562,        // 45
    313,   179, -1016,        // 48
   1100,   649,  -479,        // 51
    223,   647,   629,        // 54
    287,    95,  1026,        // 57
    313,   179,  1158,        // 60
    250,   794,   705,        // 63
   1079,   835,    75,        // 66
    223,   833,    75,        // 69
   1100,   649,   629,        // 72
   1163,   101,  1026,        // 75
   1247,  -501,  1120,        // 78
    331,  -501,  1120,        // 81
    230,   986,    71,        // 84
    354,  -822, -1123,        // 87
    357,  -502, -1123,        // 90
   -357,  -502, -1123,        // 93
   -354,  -822, -1123,        // 96
    357,  -502,  1265,        // 99
   1334,   531,   532,        // 102
   1389,    48,   860,        // 105
   1156,   172, -1009,        // 108
   1086,   781,  -557,        // 111
   1389,    48,  -709,        // 114
   1334,   531,  -382,        // 117
   1484,   549,  -382,        // 120
   1539,    66,  -709,        // 123
   1466,  -509,   937,        // 126
   1617,  -509,   937,        // 129
   1451,   823,  -557,        // 132
   1521,   214, -1009,        // 135
   1466,  -509,  -787,        // 138
   1617,  -509,  -787,        // 141
   1465,   712,    75,        // 144
   1428,  1028,    75,        // 147
   1621,  -501,  1273,        // 150
   1621,  -501, -1123,        // 153
   1315,   695,    75,        // 156
   1247,  -501, -1123,        // 159
   1247,  -501,  1271,        // 162
   1156,   172,  1160,        // 165
   1086,   781,   707,        // 168
   1062,   986,    75,        // 171
    300, -1121,  -892,        // 174
    348,  -822,  -971,        // 177
    300, -1121, -1123,        // 180
  -1335, -1581,   959,        // 183
  -1335, -1581,  -808,        // 186
  -1504,  -822,  -971,        // 189
  -1504,  -822,  1122,        // 192
  -1656,  -822, -1123,        // 195
  -1656,  -822,  1273,        // 198
   -331,  -501,  -969,        // 201
   -287,    95,  -875,        // 204
  -1163,   101,  -875,        // 207
  -1247,  -501,  -969,        // 210
   -313,   179, -1016,        // 213
   -250,   794,  -562,        // 216
   -223,   647,  -479,        // 219
  -1100,   649,  -479,        // 222
   -223,   647,   629,        // 225
   -250,   794,   705,        // 228
   -313,   179,  1158,        // 231
   -287,    95,  1026,        // 234
   -223,   833,    75,        // 237
  -1079,   835,    75,        // 240
  -1100,   649,   629,        // 243
  -1163,   101,  1026,        // 246
  -1247,  -501,  1120,        // 249
   -331,  -501,  1120,        // 252
   -230,   986,    71,        // 255
  -1247,  -501, -1123,        // 258
  -1621,  -501, -1123,        // 261
   -357,  -502,  1265,        // 264
  -1334,   531,   532,        // 267
  -1484,   549,   532,        // 270
  -1539,    66,   860,        // 273
  -1389,    48,   860,        // 276
  -1086,   781,  -557,        // 279
  -1156,   172, -1009,        // 282
  -1389,    48,  -709,        // 285
  -1539,    66,  -709,        // 288
  -1484,   549,  -382,        // 291
  -1334,   531,  -382,        // 294
  -1617,  -509,   937,        // 297
  -1466,  -509,   937,        // 300
  -1466,  -509,  -787,        // 303
  -1617,  -509,  -787,        // 306
  -1465,   712,    75,        // 309
  -1315,   695,    75,        // 312
  -1521,   214, -1009,        // 315
  -1451,   823,  -557,        // 318
  -1247,  -501,  1271,        // 321
  -1621,  -501,  1273,        // 324
  -1521,   214,  1160,        // 327
  -1156,   172,  1160,        // 330
  -1451,   823,   707,        // 333
  -1086,   781,   707,        // 336
  -1062,   986,    75,        // 339
  -1428,  1028,    75,        // 342
   -300, -1121,  -892,        // 345
   -300, -1121, -1123,        // 348
   -348,  -822,  -971         // 351
}; // chestVertices

#define CHEST_TRIANGLE_COUNT 232
const S3L_Index chestTriangleIndices[CHEST_TRIANGLE_COUNT * 3] = {
      1,     3,     0,        // 0
      1,     5,     2,        // 3
      7,     9,     6,        // 6
     10,    12,    13,        // 9
     14,    16,    13,        // 12
     13,    17,    14,        // 15
     18,    20,    21,        // 18
     14,    22,    23,        // 21
     18,    22,    24,        // 24
     19,    24,    25,        // 27
     27,    25,    26,        // 30
     23,    15,    14,        // 33
     30,    32,    29,        // 36
     13,    30,    10,        // 39
     19,    33,    20,        // 42
     23,    21,    28,        // 45
     34,     8,     7,        // 48
     17,    36,    37,        // 51
     39,    41,    38,        // 54
     42,     8,    35,        // 57
     40,    45,    41,        // 60
     46,    41,    47,        // 63
     48,    44,    40,        // 66
      4,    50,    43,        // 69
      5,    47,    51,        // 72
      5,    43,    47,        // 75
     52,    40,    39,        // 78
     52,     7,    48,        // 81
     53,    45,    36,        // 84
      6,    48,     7,        // 87
     36,    44,    37,        // 90
     26,    33,    27,        // 93
     30,    11,    10,        // 96
     54,     9,    50,        // 99
     55,     6,     9,        // 102
     37,    49,    57,        // 105
     56,    49,     6,        // 108
     41,    51,    47,        // 111
     24,    55,    25,        // 114
     22,    37,    57,        // 117
     12,    53,    36,        // 120
     22,    56,    24,        // 123
     25,    54,    26,        // 126
     46,    43,    42,        // 129
     35,    39,    38,        // 132
     52,    39,    34,        // 135
     46,    35,    38,        // 138
     59,    60,    58,        // 141
     62,    64,    61,        // 144
     65,    64,    63,        // 147
     67,    69,    70,        // 150
     71,    73,    68,        // 153
     68,    74,    69,        // 156
     75,    77,    78,        // 159
     73,    80,    74,        // 162
     80,    75,    81,        // 165
     81,    78,    82,        // 168
     82,    84,    83,        // 171
     72,    79,    73,        // 174
     86,    87,    65,        // 177
     72,    16,    15,        // 180
     71,    30,    16,        // 183
     31,    68,    67,        // 186
     78,    88,    84,        // 189
     79,    76,    75,        // 192
     89,    91,    92,        // 195
     74,    94,    69,        // 198
     96,    98,    95,        // 201
     91,   100,    92,        // 204
    101,    96,    95,        // 207
     65,    99,    66,        // 210
     98,   103,   104,        // 213
    103,    89,   104,        // 216
    105,    86,    94,        // 219
    106,    94,    93,        // 222
     83,    88,   107,        // 225
     70,    31,    67,        // 228
    107,   109,   110,        // 231
    110,   111,   112,        // 234
    114,    93,   113,        // 237
    111,   113,   112,        // 240
    110,    81,    82,        // 243
     80,    93,    74,        // 246
     69,    86,    70,        // 249
    112,    80,    81,        // 252
    107,    82,    83,        // 255
     99,   101,   100,        // 258
     98,    92,    95,        // 261
    104,    89,    98,        // 264
     95,   100,   101,        // 267
     32,    86,    65,        // 270
    116,   117,   115,        // 273
     15,    85,    72,        // 276
     21,    85,    28,        // 279
     20,    76,    21,        // 282
     33,    77,    20,        // 285
     29,   116,    60,        // 288
    116,    58,    60,        // 291
     62,    58,   115,        // 294
      2,    58,     3,        // 297
     63,   115,   117,        // 300
      0,    64,     1,        // 303
      3,    61,     0,        // 306
     29,    53,    30,        // 309
     53,     5,    51,        // 312
     50,     4,    54,        // 315
     54,     4,    33,        // 318
     66,   107,    88,        // 321
    108,   107,    66,        // 324
     33,    66,    88,        // 327
      1,    66,     4,        // 330
     32,    63,   117,        // 333
      2,    29,    59,        // 336
     66,    99,   108,        // 339
     91,   108,    99,        // 342
     90,   109,    91,        // 345
    103,   111,    90,        // 348
    103,   106,   114,        // 351
     97,   105,   106,        // 354
     96,    87,   105,        // 357
     87,   102,    65,        // 360
     50,     8,    43,        // 363
      1,     2,     3,        // 366
      1,     4,     5,        // 369
      7,     8,     9,        // 372
     10,    11,    12,        // 375
     14,    15,    16,        // 378
     13,    12,    17,        // 381
     18,    19,    20,        // 384
     14,    17,    22,        // 387
     18,    23,    22,        // 390
     19,    18,    24,        // 393
     27,    19,    25,        // 396
     23,    28,    15,        // 399
     30,    31,    32,        // 402
     13,    16,    30,        // 405
     19,    27,    33,        // 408
     23,    18,    21,        // 411
     34,    35,     8,        // 414
     17,    12,    36,        // 417
     39,    40,    41,        // 420
     42,    43,     8,        // 423
     40,    44,    45,        // 426
     46,    38,    41,        // 429
     48,    49,    44,        // 432
      5,     4,    43,        // 435
     52,    48,    40,        // 438
     52,    34,     7,        // 441
     53,    51,    45,        // 444
      6,    49,    48,        // 447
     36,    45,    44,        // 450
     26,    54,    33,        // 453
     30,    53,    11,        // 456
     54,    55,     9,        // 459
     55,    56,     6,        // 462
     37,    44,    49,        // 465
     56,    57,    49,        // 468
     41,    45,    51,        // 471
     24,    56,    55,        // 474
     22,    17,    37,        // 477
     12,    11,    53,        // 480
     22,    57,    56,        // 483
     25,    55,    54,        // 486
     46,    47,    43,        // 489
     35,    34,    39,        // 492
     46,    42,    35,        // 495
     59,    29,    60,        // 498
     62,    63,    64,        // 501
     65,    66,    64,        // 504
     67,    68,    69,        // 507
     71,    72,    73,        // 510
     68,    73,    74,        // 513
     75,    76,    77,        // 516
     73,    79,    80,        // 519
     80,    79,    75,        // 522
     81,    75,    78,        // 525
     82,    78,    84,        // 528
     72,    85,    79,        // 531
     72,    71,    16,        // 534
     71,    31,    30,        // 537
     31,    71,    68,        // 540
     78,    77,    88,        // 543
     79,    85,    76,        // 546
     89,    90,    91,        // 549
     74,    93,    94,        // 552
     96,    97,    98,        // 555
     91,    99,   100,        // 558
    101,   102,    96,        // 561
     65,   102,    99,        // 564
     98,    97,   103,        // 567
    103,    90,    89,        // 570
    105,    87,    86,        // 573
    106,   105,    94,        // 576
     83,    84,    88,        // 579
     70,    86,    31,        // 582
    107,   108,   109,        // 585
    110,   109,   111,        // 588
    114,   106,    93,        // 591
    111,   114,   113,        // 594
    110,   112,    81,        // 597
     80,   113,    93,        // 600
     69,    94,    86,        // 603
    112,   113,    80,        // 606
    107,   110,    82,        // 609
     99,   102,   101,        // 612
     98,    89,    92,        // 615
     95,    92,   100,        // 618
     32,    31,    86,        // 621
    116,    32,   117,        // 624
     15,    28,    85,        // 627
     21,    76,    85,        // 630
     20,    77,    76,        // 633
     33,    88,    77,        // 636
     29,    32,   116,        // 639
    116,   115,    58,        // 642
     62,     3,    58,        // 645
      2,    59,    58,        // 648
     63,    62,   115,        // 651
      0,    61,    64,        // 654
      3,    62,    61,        // 657
     29,     5,    53,        // 660
     33,     4,    66,        // 663
      1,    64,    66,        // 666
     32,    65,    63,        // 669
      2,     5,    29,        // 672
     91,   109,   108,        // 675
     90,   111,   109,        // 678
    103,   114,   111,        // 681
    103,    97,   106,        // 684
     97,    96,   105,        // 687
     96,   102,    87,        // 690
     50,     9,     8         // 693
}; // chestTriangleIndices

#define CHEST_UV_COUNT 154
const S3L_Unit chestUVs[CHEST_UV_COUNT * 2] = {
     23,   196,         // 0
     71,   313,         // 2
     71,   196,         // 4
    307,   495,         // 6
    495,   483,         // 8
    482,   495,         // 10
    343,   350,         // 12
    286,   392,         // 14
    322,   325,         // 16
    151,   185,         // 18
    198,   247,         // 20
    198,   185,         // 22
    250,   185,         // 24
    198,   172,         // 26
    250,   247,         // 28
    347,   185,         // 30
    397,   172,         // 32
    347,   172,         // 34
    298,   247,         // 36
    298,   185,         // 38
    347,   247,         // 40
    397,   185,         // 42
    397,   247,         // 44
    436,   185,         // 46
    436,   247,         // 48
    250,   172,         // 50
    143,   172,         // 52
    122,   124,         // 54
    122,   172,         // 56
    448,   172,         // 58
    298,   172,         // 60
    353,   360,         // 62
    316,   400,         // 64
    198,   260,         // 66
    250,   260,         // 68
    426,   358,         // 70
    469,   396,         // 72
    452,   400,         // 74
    332,   444,         // 76
    330,   402,         // 78
    435,   345,         // 80
    500,   385,         // 82
    454,   444,         // 84
    465,   456,         // 86
    387,   330,         // 88
    454,   317,         // 90
    294,   483,         // 92
    293,   460,         // 94
    322,   457,         // 96
    496,   459,         // 98
    389,   345,         // 100
    143,   260,         // 102
    198,   283,         // 104
    250,   283,         // 106
    151,   247,         // 108
    448,   260,         // 110
    397,   283,         // 112
    448,   283,         // 114
    397,   260,         // 116
    347,   283,         // 118
    298,   283,         // 120
    298,   260,         // 122
    347,   260,         // 124
    109,   178,         // 126
    105,   158,         // 128
     90,   162,         // 130
    270,   313,         // 132
    203,   495,         // 134
    270,   495,         // 136
    495,   483,         // 138
    307,   495,         // 140
    482,   495,         // 142
    151,   111,         // 144
    198,    51,         // 146
    151,    51,         // 148
    198,   124,         // 150
    250,   111,         // 152
    198,   111,         // 154
    250,    51,         // 156
    347,   111,         // 158
    397,   124,         // 160
    397,   111,         // 162
    298,    51,         // 164
    347,    51,         // 166
    397,    51,         // 168
    436,   111,         // 170
    436,    51,         // 172
    250,   124,         // 174
    298,   111,         // 176
    143,    40,         // 178
    143,    13,         // 180
    122,    13,         // 182
    143,   124,         // 184
    448,   124,         // 186
    347,   124,         // 188
    353,   360,         // 190
    317,   400,         // 192
    331,   402,         // 194
    198,    40,         // 196
    469,   396,         // 198
    427,   358,         // 200
    452,   400,         // 202
    333,   444,         // 204
    455,   444,         // 206
    322,   457,         // 208
    295,   483,         // 210
    388,   330,         // 212
    389,   345,         // 214
    198,    13,         // 216
    250,    13,         // 218
    250,    40,         // 220
    448,    40,         // 222
    397,    13,         // 224
    397,    40,         // 226
    347,    13,         // 228
    347,    40,         // 230
    298,    13,         // 232
    298,    40,         // 234
    105,   138,         // 236
    111,   119,         // 238
     90,   133,         // 240
    298,   124,         // 242
     71,   495,         // 244
     39,   385,         // 246
     39,   424,         // 248
     23,   313,         // 250
     23,   495,         // 252
     23,   427,         // 254
     23,    11,         // 256
    183,   495,         // 258
    183,   313,         // 260
    122,   283,         // 262
    143,   283,         // 264
    471,   283,         // 266
    470,    13,         // 268
    448,    13,         // 270
    482,   283,         // 272
    111,    26,         // 274
    109,   274,         // 276
    294,   460,         // 278
    343,   350,         // 280
    287,   392,         // 282
    323,   325,         // 284
    454,   317,         // 286
    386,   297,         // 288
    436,   345,         // 290
    500,   385,         // 292
    496,   459,         // 294
    465,   456,         // 296
    385,   297,         // 298
    203,   313,         // 300
     23,   382,         // 302
     71,    11,         // 304
    482,    13          // 306
}; // chestUVs

#define CHEST_UV_INDEX_COUNT 232
const S3L_Index chestUVIndices[CHEST_UV_INDEX_COUNT * 3] = {
      0,     1,     2,        // 0
      3,     4,     5,        // 3
      6,     7,     8,        // 6
      9,    10,    11,        // 9
     12,    13,    11,        // 12
     11,    14,    12,        // 15
     15,    16,    17,        // 18
     12,    18,    19,        // 21
     15,    18,    20,        // 24
     21,    20,    22,        // 27
     23,    22,    24,        // 30
     19,    25,    12,        // 33
     26,    27,    28,        // 36
     11,    26,     9,        // 39
     21,    29,    16,        // 42
     19,    17,    30,        // 45
     31,    32,     6,        // 48
     14,    33,    34,        // 51
     35,    36,    37,        // 54
     38,    32,    39,        // 57
     40,    41,    36,        // 60
     42,    36,    43,        // 63
     44,    45,    40,        // 66
     46,    47,    48,        // 69
      4,    43,    49,        // 72
      4,    48,    43,        // 75
     50,    40,    35,        // 78
     50,     6,    44,        // 81
     51,    52,    33,        // 84
      8,    44,     6,        // 87
     33,    53,    34,        // 90
     24,    29,    23,        // 93
     26,    54,     9,        // 96
     55,    56,    57,        // 99
     58,    59,    56,        // 102
     34,    60,    61,        // 105
     62,    60,    59,        // 108
     36,    49,    43,        // 111
     20,    58,    22,        // 114
     18,    34,    61,        // 117
     10,    51,    33,        // 120
     18,    62,    20,        // 123
     22,    55,    24,        // 126
     42,    48,    38,        // 129
     39,    35,    37,        // 132
     50,    35,    31,        // 135
     42,    39,    37,        // 138
     63,    64,    65,        // 141
     66,    67,    68,        // 144
     69,    70,    71,        // 147
     72,    73,    74,        // 150
     75,    76,    77,        // 153
     77,    78,    73,        // 156
     79,    80,    81,        // 159
     76,    82,    78,        // 162
     82,    79,    83,        // 165
     83,    81,    84,        // 168
     84,    85,    86,        // 171
     87,    88,    76,        // 174
     89,    90,    91,        // 177
     87,    13,    25,        // 180
     75,    26,    13,        // 183
     92,    77,    72,        // 186
     81,    93,    85,        // 189
     88,    94,    79,        // 192
     95,    96,    97,        // 195
     78,    98,    73,        // 198
     99,   100,   101,        // 201
     96,   102,    97,        // 204
    103,    99,   101,        // 207
     69,   104,   105,        // 210
    100,   106,   107,        // 213
    106,    95,   107,        // 216
    108,    89,    98,        // 219
    109,    98,   110,        // 222
     86,    93,   111,        // 225
     74,    92,    72,        // 228
    111,   112,   113,        // 231
    113,   114,   115,        // 234
    116,   110,   117,        // 237
    114,   117,   115,        // 240
    113,    83,    84,        // 243
     82,   110,    78,        // 246
     73,    89,    74,        // 249
    115,    82,    83,        // 252
    111,    84,    86,        // 255
    104,   103,   102,        // 258
    100,    97,   101,        // 261
    107,    95,   100,        // 264
    101,   102,   103,        // 267
     27,    89,    91,        // 270
    118,   119,   120,        // 273
     25,   121,    87,        // 276
     17,   121,    30,        // 279
     16,    94,    17,        // 282
     29,    80,    16,        // 285
     28,   118,    64,        // 288
    118,    65,    64,        // 291
    122,   123,   124,        // 294
    125,   123,     1,        // 297
    126,   124,   127,        // 300
      2,   128,     0,        // 303
      1,   129,   130,        // 306
     28,    51,    26,        // 309
     51,   131,   132,        // 312
     57,   133,    55,        // 315
     55,   133,    29,        // 318
    134,   111,    93,        // 321
    135,   111,   134,        // 324
     29,   134,    93,        // 327
    136,   134,   133,        // 330
     27,   137,   119,        // 333
    138,    28,    63,        // 336
    105,   104,   139,        // 339
     96,   139,   104,        // 342
    140,   141,    96,        // 345
    106,   142,   140,        // 348
    106,   143,   144,        // 351
    145,   146,   143,        // 354
     99,   147,   146,        // 357
    147,   148,    69,        // 360
     47,    32,    48,        // 363
      0,   125,     1,        // 366
      3,    46,     4,        // 369
      6,    32,     7,        // 372
      9,    54,    10,        // 375
     12,    25,    13,        // 378
     11,    10,    14,        // 381
     15,    21,    16,        // 384
     12,    14,    18,        // 387
     15,    19,    18,        // 390
     21,    15,    20,        // 393
     23,    21,    22,        // 396
     19,    30,    25,        // 399
     26,    92,    27,        // 402
     11,    13,    26,        // 405
     21,    23,    29,        // 408
     19,    15,    17,        // 411
     31,    39,    32,        // 414
     14,    10,    33,        // 417
     35,    40,    36,        // 420
     38,    48,    32,        // 423
     40,    45,    41,        // 426
     42,    37,    36,        // 429
     44,   149,    45,        // 432
      4,    46,    48,        // 435
     50,    44,    40,        // 438
     50,    31,     6,        // 441
     51,   132,    52,        // 444
      8,   149,    44,        // 447
     33,    52,    53,        // 450
     24,    55,    29,        // 453
     26,    51,    54,        // 456
     55,    58,    56,        // 459
     58,    62,    59,        // 462
     34,    53,    60,        // 465
     62,    61,    60,        // 468
     36,    41,    49,        // 471
     20,    62,    58,        // 474
     18,    14,    34,        // 477
     10,    54,    51,        // 480
     18,    61,    62,        // 483
     22,    58,    55,        // 486
     42,    43,    48,        // 489
     39,    31,    35,        // 492
     42,    38,    39,        // 495
     63,    28,    64,        // 498
     66,   150,    67,        // 501
     69,   105,    70,        // 504
     72,    77,    73,        // 507
     75,    87,    76,        // 510
     77,    76,    78,        // 513
     79,    94,    80,        // 516
     76,    88,    82,        // 519
     82,    88,    79,        // 522
     83,    79,    81,        // 525
     84,    81,    85,        // 528
     87,   121,    88,        // 531
     87,    75,    13,        // 534
     75,    92,    26,        // 537
     92,    75,    77,        // 540
     81,    80,    93,        // 543
     88,   121,    94,        // 546
     95,   140,    96,        // 549
     78,   110,    98,        // 552
     99,   145,   100,        // 555
     96,   104,   102,        // 558
    103,   148,    99,        // 561
     69,   148,   104,        // 564
    100,   145,   106,        // 567
    106,   140,    95,        // 570
    108,    90,    89,        // 573
    109,   108,    98,        // 576
     86,    85,    93,        // 579
     74,    89,    92,        // 582
    111,   135,   112,        // 585
    113,   112,   114,        // 588
    116,   109,   110,        // 591
    114,   116,   117,        // 594
    113,   115,    83,        // 597
     82,   117,   110,        // 600
     73,    98,    89,        // 603
    115,   117,    82,        // 606
    111,   113,    84,        // 609
    104,   148,   103,        // 612
    100,    95,    97,        // 615
    101,    97,   102,        // 618
     27,    92,    89,        // 621
    118,    27,   119,        // 624
     25,    30,   121,        // 627
     17,    94,   121,        // 630
     16,    80,    94,        // 633
     29,    93,    80,        // 636
     28,    27,   118,        // 639
    118,   120,    65,        // 642
    122,     1,   123,        // 645
    125,   151,   123,        // 648
    126,   122,   124,        // 651
      2,   152,   128,        // 654
      1,   122,   129,        // 657
     28,   131,    51,        // 660
     29,   133,   134,        // 663
    136,   153,   134,        // 666
     27,    91,   137,        // 669
    138,   131,    28,        // 672
     96,   141,   139,        // 675
    140,   142,   141,        // 678
    106,   144,   142,        // 681
    106,   145,   143,        // 684
    145,    99,   146,        // 687
     99,   148,   147,        // 690
     47,     7,    32         // 693
}; // chestUVIndices

S3L_Model3D chestModel;

void chestModelInit(void)
{
  S3L_model3DInit(
    chestVertices,
    CHEST_VERTEX_COUNT,
    chestTriangleIndices,
    CHEST_TRIANGLE_COUNT,
    &chestModel);
}

#endif // guard