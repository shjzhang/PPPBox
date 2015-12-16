function diff_freq()


% 
% fix.kin.east
%
filename='/Users/shjzhang/Documents/Data/IGS/data/fix.kin.east.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);


bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;

set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in east [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% fix.kin.north
%
filename='/Users/shjzhang/Documents/Data/IGS/data/fix.kin.north.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;
set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in north [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% fix.kin.up
%
filename='/Users/shjzhang/Documents/Data/IGS/data/fix.kin.up.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;

set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in up [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% float.kin.east
%
filename='/Users/shjzhang/Documents/Data/IGS/data/float.kin.east.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);


bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;

set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in east [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% float.kin.north
%
filename='/Users/shjzhang/Documents/Data/IGS/data/float.kin.north.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;
set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in north [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% float.kin.up
%
filename='/Users/shjzhang/Documents/Data/IGS/data/float.kin.up.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;

set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in up [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% fix.static.east
%
filename='/Users/shjzhang/Documents/Data/IGS/data/fix.static.east.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);


bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;

set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in east [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% fix.static.north
%
filename='/Users/shjzhang/Documents/Data/IGS/data/fix.static.north.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;
set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in north [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% fix.static.up
%
filename='/Users/shjzhang/Documents/Data/IGS/data/fix.static.up.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;

set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in up [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% float.static.east
%
filename='/Users/shjzhang/Documents/Data/IGS/data/float.static.east.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);


bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;

set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in east [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% float.static.north
%
filename='/Users/shjzhang/Documents/Data/IGS/data/float.static.north.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;
set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in north [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% float.static.up
%
filename='/Users/shjzhang/Documents/Data/IGS/data/float.static.up.rms';
diff=load(filename);

% convert unit from meters to mm. 
diff=diff.*100;

bin=-(5*40+2.5):5:(5*40+2.5);

[counts, binValues] = hist(diff, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);

grid on;

set(gca,'Ygrid','on') 
set(gca,'XTick',-100:20:100);
axis([-100 100 0 100]);
xlabel('Difference in up [mm]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 

