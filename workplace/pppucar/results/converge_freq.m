function converge_freq()


% 
% fix.kin.converge
%
filename='/Users/shjzhang/Documents/Data/IGS/data/converge.fix.kin.txt';
converge=load(filename);

% convert time from seconds to minutes
converge=converge./60;

bin=0:10:120;

[counts, binValues] = hist(converge, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);
axis([0 120 0 100]);
xlabel('Time to fixed solutions [minutes]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% fix.static.converge
%
filename='/Users/shjzhang/Documents/Data/IGS/data/converge.fix.static.txt';
converge=load(filename);

% convert time from seconds to minutes
converge=converge./60;

bin=0:10:120;

[counts, binValues] = hist(converge, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);
axis([0 120 0 100]);
xlabel('Time to fixed solutions [minutes]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% float.kin.converge
%
filename='/Users/shjzhang/Documents/Data/IGS/data/converge.float.kin.txt';
converge=load(filename);

% convert time from seconds to minutes
converge=converge./60;

bin=0:10:120;

[counts, binValues] = hist(converge, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);
axis([0 120 0 100]);
xlabel('Time to fixed solutions [minutes]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


% 
% float.static.converge
%
filename='/Users/shjzhang/Documents/Data/IGS/data/converge.float.static.txt';
converge=load(filename);

% convert time from seconds to minutes
converge=converge./60;

bin=0:10:120;

[counts, binValues] = hist(converge, bin);

normalizedCounts = 100 * counts / sum(counts);

grid on;

bar(binValues, normalizedCounts, 'barwidth', 1);
axis([0 120 0 100]);
xlabel('Time to fixed solutions [minutes]');
ylabel('Percentage [%]'); 


set(gcf, 'PaperUnits','centimeters','PaperSize', [8 6],'PaperPosition',[0 0 8 6]);
set(gca, 'Fontsize', 7);

set(get(gca,'XLabel'),'FontSize',6,'Vertical','top');
set(get(gca,'YLabel'),'FontSize',6,'Vertical','middle');
set(gca,'XMinorTick','on')
set(gca,'YMinorTick','on')
saveas(gcf,strcat(filename,'.png'),'png') 


