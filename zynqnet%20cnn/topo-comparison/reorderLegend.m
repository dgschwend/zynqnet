function out = reorderLegend(order,h)
% reorderLegend 
%   Re-orders the legend entries 
%   (without modifying the actual order the curves are stacked on the axes)
%
%   order  - an array specifying the order of legend entries
%   h      - axis handle (optional, defaults to current axis)
%
%   Invoking the function without an argument or with order==[] 
%   causes the labels of the curves to be returned (or displayed, if
%   nargout==0).
%       e.g.
%           >> x = 0:.1:10;
%           >> plot(x,x,x,2*x,x,3*x,x,4*x); legend('x','2x','3x','4x');
%           >> reorderLegend
%           [1] 4x
%           [2] 3x
%           [3] 2x
%           [4] x 
%
%   The indices of the labels (i.e., 1 to 4 in the above e.g.)
%   can be used to specify the desired order of legend entries.
%       e.g.
%           >> reorderLegend([4,2,1,3]); 
%               % legend: 'x','3x','4x','2x' (from top to bottom)
%
%   Notes: 
%   - By default, the legend entries are in the reverse order of the
%   indices (because the curves are in a stack).
%   - `reorderLegend' doesn't change the order of the curves
%   (i.e., the indices are not changed when it's called).
%
%   Author        : Damith Senaratne, (http://www.damiths.info)
%   Released date : 21th June 2012 
    
    if nargin < 2
        h = gca;
    end
     
    hC = findobj(h,'Type','line');
    
    if nargin == 0 || isempty(order)
        % output/display the internal order
        lbl = get(hC,{'DisplayName'});        
        if nargout == 1 
            out = lbl;
        else
            for k = 1:length(lbl)
               display([sprintf('[%*d] ',fix(log10(length(lbl)))+1,k) lbl{k}]);
            end
        end
        
    else
       % reorder the legend entries 
       legend(hC(order)); 
    end
end