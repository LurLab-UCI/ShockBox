function preview1_fcn(himage)
% Example update preview window function.

% Get timestamp for frame.
% tstampstr = event.FrameRate;

% Get handle to text label uicontrol.
ht = getappdata(himage,'HandleToTimestampLabel');

% Set the value of the text label.
ht.String = event.FrameRate;

himage.CData = event.Data;
end