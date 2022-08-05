function   init();

global arduino vid;


delete(instrfindall);
imaqreset;
devs = fcom();
[r,c] = size(devs);

for i = 1:r
    [str,n] =  devs{i,:};
    if ~isempty(str)
        if contains(string(str),'Arduino') &&  contains(string(str),'Due')
            break;
        end
    end
end
comstr = ['COM' num2str(n)]; % ,'Mega'];
arduino = serial(comstr,'BaudRate',115200); % create serial communication object on port COM4
arduino.BytesAvailableFcnCount = 5;
arduino.BytesAvailableFcnMode = 'byte';
fopen(arduino); % initiate arduino communication

% wait until data from arduino
tic;
while 1
    bav = get(arduino,'BytesAvailable');
    if bav > 0
        sdata = fread(arduino,bav);    
        fprintf('init %4.3f %s \n',toc,sdata);   %GL edit added \n to string
        if contains(char(sdata'),'Rst')
            break
        end
    end
end


arduino.BytesAvailableFcn = {'read_serial'};

fprintf('Reset received\n');
try

    vid = videoinput('winvideo', 2, 'MJPG_640x480');
    src = getselectedsource(vid);
    vid.FramesPerTrigger = Inf;
catch
    fprintf('No Camera Found\n');
end
