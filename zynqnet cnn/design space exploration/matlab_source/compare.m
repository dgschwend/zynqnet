function ax = compare( title_t, names, var1, label1, var2, label2)

    %% Check for Consistency...
    nn = length(var1);
    if (length(var2) ~= nn || length(names) ~= nn)
        disp('Error in Configuration. Make sure to select comparable values!');
        return;
    end

    %% Plot

    hold on;
    for i = 1:nn
        randomdev = 1/40;
        ax = plot(var2(i)*(1+rand()*randomdev-randomdev/2),var1(i)*(1+rand()*randomdev-randomdev/2), 'o', ...
            'MarkerSize', 10, ...
            'MarkerFaceColor', hsv2rgb([i./nn, 1, .8]), ...
            'MarkerEdgeColor', hsv2rgb([mod(i./nn*5.5, 1), 1, .5]));
    end
    hold off;

    ylabel(label1);
    xlabel(label2);
    title(title_t);

end

