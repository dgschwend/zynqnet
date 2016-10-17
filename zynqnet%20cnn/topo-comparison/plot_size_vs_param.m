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

numfigs = 1;

ax0 = subplot(1,numfigs,1);
[tit0, ~] = compare(sprintf('CNN Topologies: Param Size vs. Computational Complexity'),...
            name(1:nitem),...
            1e6*4/1024/1024*params(1:nitem), 'Parameter Size [MB]', ...
            1/1000*maccs(1:nitem), 'GMACC/s [log]');
ylim manual;
xlim(ax0, [0.1 100]);   
set(ax0,'xscale','log');

ylim(ax0, [0.1 10000]); 
set(ax0,'yscale','log');

set(ax0, 'XTickLabels', get(ax0, 'XTick'));
set(ax0, 'YTickLabels', get(ax0, 'YTick'));

% Move Axes out of way for legend
set(ax0, 'Position', get(ax0, 'Position') - [0,-0.15,0,0.16]);

lblsize = 11;

%%% LEGEND
hL = columnlegend(6, name(1:nitem), 'Location', 'NorthWest', 'boxoff', 'FontSize', lblsize, 'FontName', 'Palatino');
set(hL, 'Position',  [.1 -0.62 .9 .8]);
set(hL, 'FontSize',  lblsize-4);
set(hL, 'FontName', 'Palatino');

%%% Set Figure Size to good scale for export
set(gcf, 'Position', [200,600,800,500]);

%%% Decrease label Fonts
set(ax0.XLabel, 'FontSize', lblsize-2); set(ax0.YLabel, 'FontSize', lblsize);
set(tit0, 'FontSize', lblsize+2); set(tit0, 'FontWeight', 'light'); 

%%% Increase Title Distance
set(tit0, 'Units', 'centimeters');  % avoid that title move with graphs
set(tit0, 'Position', get(tit0, 'Position') + [0 .35 0]);

%%% Keep Labels updated on Plots 2 and 3
%%% (use 1 10 100 1000 instead of 10^(0 1 2 3)
% while true
%     set(ax1, 'XTickLabels', get(ax1, 'XTick'));
%     set(ax2, 'XTickLabels', get(ax2, 'XTick'));
%     drawnow;
%     % Print Preview
%     load printtemplate.mat
%     setprinttemplate(gcf, printtemplate)
%     printpreview(gcf);
%     pause(1);
% end