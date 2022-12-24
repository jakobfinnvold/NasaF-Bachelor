%% ptsematest
clear; close all; clc; 

T = readtable('datalog_regular_ptsematest.txt'); 

max_0 = T(5:8:end,:);
max_1 = T(6:8:end,:);
max_2 = T(7:8:end,:);
max_3 = T(8:8:end,:);

figure
plot(max_0.Var11, LineWidth=2)
xlabel('Iterations')
ylabel('Time in us')

hold on
plot(max_1.Var11, LineWidth=2)

hold on
plot(max_2.Var11, LineWidth=1.5)

hold on
plot(max_3.Var11, LineWidth=1.5)

legend('CPU 0', 'CPU 1', 'CPU 2', 'CPU 3')
title('ptsematest regular kernel 5.10.90')

%% sigwaittest
clear; close all; clc;

T = readtable('datalog_regular_sigwaittest.txt');

max_0 = T(5:8:end,:);
max_1 = T(6:8:end,:);
max_2 = T(7:8:end,:);
max_3 = T(8:8:end,:);

figure
plot(max_0.Var11, LineWidth=2)
xlabel('Iterations')
ylabel('Time in us')

hold on
plot(max_1.Var11, LineWidth=2)

hold on
plot(max_2.Var11, LineWidth=1.5)

hold on
plot(max_3.Var11, LineWidth=1.5)

legend('CPU 0', 'CPU 1', 'CPU 2', 'CPU 3')
title('sigwaittest regular kernel 5.10.90')


%% sigwaittest realtime
clear; close all; clc;

T = readtable('datalog_rt_sigwaittest.txt');


max_0 = T(5:8:end,:);
max_1 = T(6:8:end,:);
max_2 = T(7:8:end,:);
max_3 = T(8:8:end,:);

figure
plot(max_0.Var11, LineWidth=2)
xlabel('Iterations')
ylabel('Time in us')

hold on
plot(max_1.Var11, LineWidth=2)

hold on
plot(max_2.Var11, LineWidth=1.5)

hold on
plot(max_3.Var11, LineWidth=1.5)

legend('CPU 0', 'CPU 1', 'CPU 2', 'CPU 3')
title('sigwaittest rt-kernel 5.10.90-rt61')

%% svsematest
clear; close all; clc;
T = readtable('datalog_regular_svsematest.txt');

max_0 = T(5:8:end,:);
max_1 = T(6:8:end,:);
max_2 = T(7:8:end,:);
max_3 = T(8:8:end,:);

figure
plot(max_0.Var11, LineWidth=2)
xlabel('Iterations')
ylabel('Time in us')

hold on
plot(max_1.Var11, LineWidth=2)

hold on
plot(max_2.Var11, LineWidth=1.5)

hold on
plot(max_3.Var11, LineWidth=1.5)

legend('CPU 0', 'CPU 1', 'CPU 2', 'CPU 3')
title('svsematest regular kernel 5.10.90')


%% svsematest rt
clear; close all; clc;
T = readtable('datalog_rt_svsematest.txt');

max_0 = T(5:8:end,:);
max_1 = T(6:8:end,:);
max_2 = T(7:8:end,:);
max_3 = T(8:8:end,:);

figure
plot(max_0.Var11, LineWidth=2)
xlabel('Iterations')
ylabel('Time in us')

hold on
plot(max_1.Var11, LineWidth=2)

hold on
plot(max_2.Var11, LineWidth=1.5)

hold on
plot(max_3.Var11, LineWidth=1.5)

legend('CPU 0', 'CPU 1', 'CPU 2', 'CPU 3')
title('svsematest rt-kernel 5.10.90-rt61')

%% N-Queens problem regular kernel
clear; close all; clc;
T = readtable('multithread_output_regular');

temp = T.cpu_temp;
time_all_solutions = 0:0.26111:2.35;
time_per_iteration = T.seconds;
yyaxis left
scatter(time_all_solutions, time_per_iteration, 'blue', 'filled')
ylabel('Time to find solutions in seconds')

hold on
yyaxis right
ylabel('CPU temperature')
plot(time_all_solutions,temp, 'red', LineWidth=2)

legend('Solution time', 'CPU Temperature')
xlabel('Total timeduration in minutes')

%% N-queens problem rt-kernel
clear; close all; clc;
T = readtable('multithread_output_rt');

temp = T.cpu_temp;
time_all_solutions = 0:0.2648111:2.3833;
time_per_iteration = T.seconds;
yyaxis left
scatter(time_all_solutions, time_per_iteration, 'blue', 'filled')
ylabel('Time to find solutions in seconds')

hold on
yyaxis right
ylabel('CPU temperature')
plot(time_all_solutions,temp, 'red', LineWidth=2)

legend('Solution time', 'CPU Temperature')
xlabel('Total timeduration in minutes')
title('N-Queens problem RT-kernel')

%% TOF-sensor readings
clear; close all; clc;
T = readtable('ToF.xltx');

reading = 0 : 1 : 15; 

acryl1 = T.Var2; 
acryl2 = T.Var3;
acryl3 = T.Var4;
acryl4 = T.Var5;
acryl5 = T.Var6; 
acryl6 = T.Var7; 
acryl7 = T.Var8;
acryl8 = T.Var9;
acryl9 = T.Var10; 


figure

plot(reading, acryl1, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl2, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl3, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl4, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl5, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl6, '.', 'MarkerSize', 10, 'Color', 'b'); 
hold on
plot(reading, acryl7, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl8, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
xlabel('Reading 0 to 15');
ylabel('Distance in mm');
title('Measurements with Acrylic glass');
plot(reading, acryl9, '.', 'MarkerSize', 10, 'Color', 'b');

%% Without acryl
clear; close all; clc;
T = readtable('ToF2.xlsx');

reading = 0 : 1 : 15; 

acryl1 = T.Var12; 
acryl2 = T.Var13;
acryl3 = T.Var14;
acryl4 = T.Var15;
acryl5 = T.Var16; 
acryl6 = T.Var17; 
acryl7 = T.Var18;
acryl8 = T.Var19;
acryl9 = T.Var20; 
acryl10 = T.Var21; 


figure

plot(reading, acryl1, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl2, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl3, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl4, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl5, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl6, '.', 'MarkerSize', 10, 'Color', 'b'); 
hold on
plot(reading, acryl7, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl8, '.', 'MarkerSize', 10, 'Color', 'b');
hold on
plot(reading, acryl10, 'Color', 'black');
xlabel('Reading 0 to 15');
ylabel('Distance in mm');
title('Measurements without Acrylic glass');
plot(reading, acryl9, '.', 'MarkerSize', 10, 'Color', 'b');