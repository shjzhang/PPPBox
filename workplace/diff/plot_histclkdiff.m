function [ output_args ] = hist_upddiff( input_args )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

filename='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/sgg_cnes.diff';
upddiff = load(filename);

PRN=[2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 25 26 27 28 29 30 31 32]

bar(PRN, upddiff(:,1));

grid on;

xlabel('PRN number ');
ylabel('WL UPD difference [Cycles] ')

axis([0 32 -0.15 0.15])
    
y=-0.15:0.05:0.15;

legend('RMS=0.03');

set(gca, 'ytick', y);

set(gcf, 'PaperUnits','centimeters','PaperSize', [9 7],'PaperPosition',[0 0 9 7]);
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);

set(get(gca,'XLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','top');
set(get(gca,'YLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','middle');

saveas(gcf,'/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/upd.diff.png','png') 

end

