function preview_Callback_n(app)

global vid preview_running;
global pfig;
 preview_running = 1;
 newhandle = 1;
 
if ~isempty(pfig)
 if ishandle(pfig)
        if isvalid(pfig)
            newhandle = 0;   % if valid handle
        end
 end
end

if newhandle
    pfig = figure('Toolbar','none',...
           'Menubar', 'none',...
           'NumberTitle','Off',...
           'Name','My Preview Window','Position',[15 440 600 600]);

    hTextLabel = uicontrol('style','text','String','Framerate', ...
        'Units','normalized',...
        'Position',[0.85 -.04 .15 .08]);
end
    
    nBands = vid.NumberOfBands;
    vr = vid.VideoResolution;
    hImage = image( zeros(vr(1),vr(2), nBands) );

    preview(vid, hImage);
    set(pfig,'visible','on');
