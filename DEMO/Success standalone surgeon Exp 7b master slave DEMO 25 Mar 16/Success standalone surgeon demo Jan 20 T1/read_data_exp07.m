% del


filename = ['CalibTorque_springtip_Translation_GW_01' '.txt' ];
fileID = fopen(filename,'r');
% formatSpec = '%f %f %f %f %f %f %f %f %f %f';
% sizeA = [10 Inf];
formatSpec = '%f %f %f %f %f %f %f %f %f %f %f %f %f';
sizeA = [13 Inf];
A = fscanf(fileID,formatSpec,sizeA);
fclose(fileID);
data=A';

% figure
% plot(data(:,7),data(:,10));
% title(' encoder pos vs time');
% xlabel('time');
% ylabel('dist (mm)');
% axis([0 inf 0 inf]);

% figure
% plot(data(:,7),data(:,8));
% title(' motor pos vs time');
% xlabel('time');
% ylabel('dist (mm)');
% axis([0 inf 0 inf]);

figure
hold on
plot(data(:,7),data(:,10),'*');
plot(data(:,7),data(:,8),'*');

plot(data(:,7),data(:,10)-data(:,8)); % error

plot(data(:,7),data(:,10));
plot(data(:,7),data(:,8));
legend('encoder dist','motor dist');
title(' pos vs time');
xlabel('time (s)');
ylabel('dist (mm)');
axis([0 inf 0 inf]);

figure
hold on
% plot(data(:,7),data(:,11),'*');
% plot(data(:,7),data(:,12),'*');
% plot(data(:,7),data(:,13),'*');

plot(data(:,7),data(:,11));
% plot(data(:,7),data(:,12));
plot(data(:,7),data(:,13));


legend('intput to motor','read from encoder');
title(' pos vs time');
xlabel('time (s)');
ylabel('dist (mm)');
axis([0 inf 0 inf]);