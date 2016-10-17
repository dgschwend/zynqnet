%-----------------------------------
% Create DSE Plots
%-----------------------------------
% David Gschwend, 2016
%-----------------------------------

%% Load Data; don't forget to reimport when changing Excel Sheets!
load all_data.mat
rng('shuffle');

%% Plot different Aspects of CNNs
figure;

%numfigs = 4;

f0 = figure;
%ax0 = plot(numfigs,1,1);
[tit0, ax0] = compare(sprintf('Top-5 Error vs. Computational Complexity'), name(1:nitem), ...
            100.*error_top5(1:nitem), 'Top-5 Error [%]', ...
            1e6.*maccs(1:nitem), 'Number of MACC Operations (log)');
ylim manual;
ylim(ax0, [0 25]);   
set(ax0,'xscale','log');
legend(name(1:nitem))

f1 = figure;
%ax1 = subplot(numfigs,1,2);
[tit1, ax1] = compare(sprintf('Top-5 Error vs. Parameter Size'), name(1:nitem), ...
            100.*error_top5(1:nitem), 'Top-5 Error [%]', ...
            1e6/1024/1024*4.*params(1:nitem), 'Parameter Size [MB] (log)');
ylim manual;
ylim(ax1, [0 25]);
xlim(ax1, [1 1000]);
set(ax1,'xscale','log');
legend(name(1:nitem))

f2 = figure
%ax2 = subplot(numfigs,1,3);
[tit2, ax2] = compare(sprintf('Top-5 Error vs. Total Activation Memory'), name(1:nitem), ...
            100.*error_top5(1:nitem), 'Top-5 Error [%]', ...
            1e6/1024/1024*4.*activations(1:nitem), 'Activation Memory [MB] (log)');
ylim manual;
ylim(ax2, [0 25]);
set(ax2,'xscale','log');
xlim(ax2, [1 1000]);

lblsize = 9;
legend(name(1:nitem))

%%% LEGEND
%hL = columnlegend(3, name(1:nitem), 'Location', 'NorthWest', 'boxoff', 'FontSize', lblsize, 'FontName', 'Palatino');
%set(hL, 'Position',  [0.06   .12    0.9    0.2]);

%%% Set Figure Size to good scale for export
%set(gcf, 'Position', [200,600,420,600]);

%%% Shrink Axes height a little...
set(ax0, 'YTick', [0 5 10 15 20 25]);
set(ax1, 'YTick', [0 5 10 15 20 25]);
set(ax2, 'YTick', [0 5 10 15 20 25]);
set(ax0, 'Position', [0.1300 0.1100 0.7750 0.8069]);
set(ax1, 'Position', get(ax1, 'Position') .* [1 1 1 .999]);
set(ax2, 'Position', get(ax2, 'Position') .* [1 1 1 .99]);

set(f0, 'Position', [360 500 700 200]);
set(f1, 'Position', [360 500 700 200]);
set(f2, 'Position', [360 500 700 200]);

%%% Decrease label Fonts
set(ax0.XLabel, 'FontSize', lblsize); set(ax0.YLabel, 'FontSize', lblsize);
set(ax1.XLabel, 'FontSize', lblsize); set(ax1.YLabel, 'FontSize', lblsize);
set(ax2.XLabel, 'FontSize', lblsize); set(ax2.YLabel, 'FontSize', lblsize);
set(tit0, 'FontSize', lblsize+2); set(tit0, 'FontWeight', 'light'); 
set(tit1, 'FontSize', lblsize+2); set(tit1, 'FontWeight', 'light'); 
set(tit2, 'FontSize', lblsize+2); set(tit2, 'FontWeight', 'light'); 

%%% Increase Title Distance
set(tit0, 'Units', 'centimeters');  % avoid that title move with graphs
set(tit1, 'Units', 'centimeters');
set(tit2, 'Units', 'centimeters');
%set(tit0, 'Position', get(tit0, 'Position') + [0 0 0]);
%set(tit1, 'Position', get(tit1, 'Position') + [0 1 0]);
%set(tit2, 'Position', get(tit2, 'Position') + [0 2 0]);

%%% Keep Labels updated on Plots 2 and 3
%%% (use 1 10 100 1000 instead of 10^(0 1 2 3)
set(ax1, 'XTickLabels', get(ax1, 'XTick'));
set(ax2, 'XTickLabels', get(ax2, 'XTick'));
drawnow;
pause(1);

% Print Preview
% load printtemplate.mat
% printtemplate.PaperType = 'A4';
% printtemplate.PaperSize = [8.2677 11.6929];
% printtemplate.PaperPosition = [0.66 0.26 7 12.5];
% printtemplate.PaperPositionMode = 'manual';
% printtemplate.PaperOrientation = 'portrait';
% printtemplate.FigSize = [6.9444 13];
% setprinttemplate(gcf, printtemplate)
% printpreview(gcf);
