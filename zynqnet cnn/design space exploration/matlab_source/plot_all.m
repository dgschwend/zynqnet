

%-----------------------------------
% Create DSE Plots
%-----------------------------------
% David Gschwend, 2016
%-----------------------------------

%% Load Data; don't forget to reimport when changing Excel Sheets!
load all_data.mat
rng('shuffle');

nitem = 10;

%% Plot different Aspects of CNNs
figure;

numfigs = 3;
nitem = 9;

ax0 = subplot(numfigs,1,1);
compare( 'CNN Topologies: Top-5 Error vs. Parameter Size', net(1:nitem), ...
            errtop5(1:nitem), 'Top-5 Error [%]', ...
            sizeMB(1:nitem), 'Number of Parameters');
ylim manual;
ylim(ax0, [0 25]);

ax = subplot(numfigs,1,2);
compare( 'CNN Topologies: Top-5 Error vs. Computational Complexity', net(1:nitem), ...
            errtop5(1:nitem), 'Top-5 Error [%]', ...
            opsbillion(1:nitem), 'MACC Operations [billions]');
ylim manual;
ylim(ax, [0 25]);   
        
ax = subplot(numfigs,1,3);
compare( 'CNN Topologies: Top-5 Error vs. Total Activation Memory', net(1:nitem), ...
            acctop5(1:nitem), 'Top-5 Error [%]', ...
            acctop1(1:nitem), 'Number of Activation Map Pixels');
ylim manual;
ylim(ax, [60 100]);



hL = legend(net(1:nitem));
set(hL,'Position', [0.73 0.2 0.2 0.66], 'Units', 'normalized');
