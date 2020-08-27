M118 sl:print_start

M201 X500.00 Y500.00 Z100.00 E5000.00 ;Setup machine max acceleration
M203 X500.00 Y500.00 Z10.00 E50.00 ;Setup machine max feedrate
M204 P500.00 R1000.00 T500.00 ;Setup Print/Retract/Travel acceleration
M205 X8.00 Y8.00 Z0.40 E5.00 ;Setup Jerk
M220 S100 ;Reset Feedrate
M221 S100 ;Reset Flowrate

M75 ;Start Print Timer and Engage Fil Sensor if USB Printing
M84 E ;Disable E Motor for probe accuracy on direct drive systems
G92 E0 ;Reset extruder distance to 0
G1 E-2 ;Retract filament to prevent blobs during probing
G92 E0 ;Reset extruder distance to 0

M117 Homing All...
G28 ;Home all axes

M117 Generating Mesh...
G28 Z ;Home Z Again
G29 ;auto bed leveling

M117 Heaters Recovering...
G4 S10 ;wait for heaters to recover


G92 E0 ; reset extruder

G1 Z2.0 F3000 ;Move Z Axis up
G1 X0.1 Y20 Z0.28 F5000.0 ;Move to start position
G1 X00.1 Y200.0 Z0.28 F1500.0 E15 ;Draw the first line
G1 X0.4 Y200.0 Z0.28 F5000.0 ;Move to side a little
G1 X0.4 Y20 Z0.28 F1500.0 E30 ;Draw the second line
G92 E0 ;Reset Extruder
G1 Z2.0 F3000 ;Move Z Axis up

M117 ; reset message