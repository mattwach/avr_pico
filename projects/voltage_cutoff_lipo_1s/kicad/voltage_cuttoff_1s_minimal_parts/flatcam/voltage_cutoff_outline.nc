(CNC Regtangle Test File)

G21                            (Units in mm) 
G90                                     (Absolute positioning)
G94                                     (Units per minute feed rate mode)

G01 F200.0                 (Move to starting position)
G01 Z10.0
G00 X0.0 Y0.0
M03 S10000.0                    (Start motor)
G01 Z1.0                                (Move to dwell height)

G01 F50.0                (Approach material)
G01 Z0.0
G01 F150.0                      (Draw rectangle)
G01 X55.0
G01 Y-26.0
G01 X0.0
G01 Y0.0

G01 F200.0
G01 Z10.0                 (Exit material)
M05                                     (Stop motor)
(Test complete, EOF)

