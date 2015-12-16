function [ output_args ] = plot_clkdiff(  )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here


% subplot(2,1,1)
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);
filename1='/Users/shjzhang/Documents/Develop/gpstk/workplace/diff/diff.txt';
data1 = load(filename1);

plot(data1(:,3)/30, data1(:,4), ...
     data1(:,3)/30, data1(:,5), ...
     data1(:,3)/30, data1(:,6), ...
     data1(:,3)/30, data1(:,7), ...
     data1(:,3)/30, data1(:,8), ...
     data1(:,3)/30, data1(:,9)); 




grid on;
axis([0 2880 -1.5 2])

x=0:240:2880;     
y=-2:0.5:2;
set(gca,'xtick',x);
set(gca, 'ytick', y);
legend('G01','G02', 'G03','G04', 'G05');
legend BOXOFF


xlabel('Epoch number (30s sampling rate ) ');
ylabel('Difference of Clock Bias [cycles] ')

set(gcf, 'PaperUnits','centimeters','PaperSize', [9 7],'PaperPosition',[0 0 9 7]);
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);

set(get(gca,'XLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','top');
set(get(gca,'YLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','middle');

saveas(gcf,'/Users/shjzhang/Documents/Develop/gpstk/workplace/diff/clkdiff.png','png') 



end





