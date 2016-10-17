%-----------------------------------
% Create DSE Plots
%-----------------------------------
% David Gschwend, 2016
%-----------------------------------

%% Load Data; don't forget to reimport when changing Excel Sheets!
load prior_work.mat
rng('shuffle');

%% Plot different Aspects of CNNs
figure;

numfigs = 3;

ax0 = subplot(1,numfigs,1);
[tit0, ~] = compare(sprintf('Error vs. Computational Complexity'), name(1:nitem), ...
            100.*error_top5(1:nitem), 'Top-5 Error [%]', ...
            1e6.*maccs(1:nitem), 'Number of MACC Operations (log)');
ylim manual;
ylim(ax0, [0 25]);   
set(ax0,'xscale','log');

ax1 = subplot(1,numfigs,2);
[tit1, ] = compare(sprintf('Error vs. Parameter Size'), name(1:nitem), ...
            100.*error_top5(1:nitem), 'Top-5 Error [%]', ...
            1e6/1024/1024*4.*params(1:nitem), 'Parameter Size [MB] (log)');
ylim manual;
ylim(ax1, [0 25]);
set(ax1,'xscale','log');

ax2 = subplot(1,numfigs,3);
[tit2, ~] = compare(sprintf('Error vs. Total Activation Memory'), name(1:nitem), ...
            100.*error_top5(1:nitem), 'Top-5 Error [%]', ...
            1e6/1024/1024*4.*activations(1:nitem), 'Activation Memory [MB] (log)');
ylim manual;
ylim(ax2, [0 25]);
set(ax2,'xscale','log');

% Move Axes out of way for legend
set(ax0, 'Position', get(ax0, 'Position') - [0,-0.15,0,0.16]);
set(ax1, 'Position', get(ax1, 'Position') - [0,-0.15,0,0.16]);
set(ax2, 'Position', get(ax2, 'Position') - [0,-0.15,0,0.16]);


lblsize = 11;

%%% LEGEND
hL = columnlegend(6, name(1:nitem), 'Location', 'NorthWest', 'boxoff', 'FontSize', lblsize, 'FontName', 'Palatino');
set(hL, 'Position',  [0.12   -0.22    1.1    0.26]);
set(hL, 'FontSize',  lblsize);
set(hL, 'FontName', 'Palatino');

%%% Set Figure Size to good scale for export
set(gcf, 'Position', [200,600,800,400]);

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
set(tit0, 'Position', get(tit0, 'Position') + [0 .35 0]);
set(tit1, 'Position', get(tit1, 'Position') + [0 .35 0]);
set(tit2, 'Position', get(tit2, 'Position') + [0 .35 0]);


%%% Keep Labels updated on Plots 2 and 3
%%% (use 1 10 100 1000 instead of 10^(0 1 2 3)
while true
    set(ax1, 'XTickLabels', get(ax1, 'XTick'));
    set(ax2, 'XTickLabels', get(ax2, 'XTick'));
    drawnow;
    % Print Preview
    load printtemplate.mat
    setprinttemplate(gcf, printtemplate)
    printpreview(gcf);
    pause(1);
end