% Real-Time MPU6050 + HMC5883L Data Logger + Plotter
% Reads accel (m/s^2), gyro (deg/s), and mag (raw) data sent from
% Arduino over serial as comma-separated values, one line per sample:
%   millis,accX,accY,accZ,gyrX,gyrY,gyrZ,magX,magY,magZ
% Stores each axis into its own array and plots accel, gyro, and mag
% live in three separate figures.

clear; close all; clc;

%% ---------------- User-defined settings ----------------
serialPort = "COM3";      % Change this to match your Arduino's port
baudRate   = 9600;        % Must match Serial.begin() value on Arduino
plotWindow = 500;         % Number of most recent samples shown (0 = show all)

%% ---------------- Open serial connection ----------------
s = serialport(serialPort, baudRate);
configureTerminator(s, "LF");   % Arduino's println() ends lines with \n
flush(s);                        % Clear any stale buffered data

disp("Connected. Close any plot window to stop logging.");

%% ---------------- Prepare storage arrays ----------------
accX = []; accY = []; accZ = [];
gyrx = []; gyry = []; gyrz = [];
magX = []; magY = []; magZ = [];
t    = [];
sampleCount = 0;

%% ---------------- Set up the live plots ----------------
figure(1);
hPlotX = plot(nan, nan, '-r', 'DisplayName', 'AccX'); hold on;
hPlotY = plot(nan, nan, '-g', 'DisplayName', 'AccY');
hPlotZ = plot(nan, nan, '-b', 'DisplayName', 'AccZ');
hold off;
legend show; grid on;
xlabel('Time (ms)'); ylabel('Acceleration (m/s^2)');
title('Real-Time MPU6050 Accelerometer Data');

figure(2);
hPX = plot(nan, nan, '-r', 'DisplayName', 'GyrX'); hold on;
hPY = plot(nan, nan, '-g', 'DisplayName', 'GyrY');
hPZ = plot(nan, nan, '-b', 'DisplayName', 'GyrZ');
hold off;
legend show; grid on;
xlabel('Time (ms)'); ylabel('Angular Velocity (deg/s)');
title('Real-Time MPU6050 Gyroscope Data');

figure(3);
hMX = plot(nan, nan, '-r', 'DisplayName', 'MagX'); hold on;
hMY = plot(nan, nan, '-g', 'DisplayName', 'MagY');
hMZ = plot(nan, nan, '-b', 'DisplayName', 'MagZ');
%ylim([-0.1 0.1]);
hold off;
legend show; grid on;
xlabel('Time (ms)'); ylabel('Raw Magnetometer Value');
title('Real-Time HMC5883L Magnetometer Data');

%% ---------------- Real-time read + plot loop ----------------
% Loop keeps running as long as ALL THREE plot windows are still open.
% Closing any one of them ends the session.
while ishandle(hPlotX) && ishandle(hPX) && ishandle(hMX)

    if s.NumBytesAvailable > 0
        line = readline(s);
        values = str2double(split(line, ","));

        % Only proceed if the line parsed into exactly 10 valid numbers
        % (1 timestamp + 3 accel + 3 gyro + 3 mag)
        if numel(values) == 10 && all(~isnan(values))
            sampleCount = sampleCount + 1;

            t(sampleCount)    = values(1);
            accX(sampleCount) = values(2);
            accY(sampleCount) = values(3);
            accZ(sampleCount) = values(4);
            gyrx(sampleCount) = values(5);
            gyry(sampleCount) = values(6);
            gyrz(sampleCount) = values(7);
            magX(sampleCount) = values(8);
            magY(sampleCount) = values(9);
            magZ(sampleCount) = values(10); %#ok<*SAGROW>

            % Decide which slice of data to display
            if plotWindow > 0 && sampleCount > plotWindow
                idxRange = (sampleCount - plotWindow + 1):sampleCount;
            else
                idxRange = 1:sampleCount;
            end

            set(hPlotX, 'XData', t(idxRange), 'YData', accX(idxRange));
            set(hPlotY, 'XData', t(idxRange), 'YData', accY(idxRange));
            set(hPlotZ, 'XData', t(idxRange), 'YData', accZ(idxRange));

            set(hPX, 'XData', t(idxRange), 'YData', gyrx(idxRange));
            set(hPY, 'XData', t(idxRange), 'YData', gyry(idxRange));
            set(hPZ, 'XData', t(idxRange), 'YData', gyrz(idxRange));

            set(hMX, 'XData', t(idxRange), 'YData', magX(idxRange));
            set(hMY, 'XData', t(idxRange), 'YData', magY(idxRange));
            set(hMZ, 'XData', t(idxRange), 'YData', magZ(idxRange));

            drawnow limitrate;
        end
    end
end

%% ---------------- Clean up ----------------
clear s;
disp("Session ended. Data stored in accX/Y/Z, gyrx/y/z, magX/Y/Z arrays.");