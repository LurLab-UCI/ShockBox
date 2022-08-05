function start_shock(app)

global arduino sdata ack ackstr vid  ncl;


ncl = 0;

fn = [app.resdir.Value  app.mouse_id.Value  '_' datestr(now,'yyyy_mm_dd__HH_MM_SS') ];
% ----------------------------------------------------------------------------------------------------------------
% save output to text file
diary( [fn  '.txt']);

diary on
fprintf('\n');
fprintf('mouse ID  ');
fprintf(' %s\n',app.mouse_id.Value);
fprintf('directory  ');
fprintf(' %s\n',app.resdir.Value);
fprintf('number of cycles  ');
fprintf(' %d\n',app.number_of_cycles.Value);
fprintf('cycle interval min max  ');
fprintf(' %3.3f  %3.3f\n',app.cycle_interval_min.Value,app.cycle_interval_max.Value);
fprintf('shock duration min max  ');
fprintf(' %3.3f  %3.3f\n',app.shock_duration_min.Value,app.shock_duration_max.Value);
fprintf('delay min max  ');
fprintf(' %3.3f  %3.3f\n',app.delay_min.Value,app.delay_max.Value);
fprintf('tone duration min max  ');
fprintf(' %3.3f  %3.3f\n',app.tone_duration_min.Value,app.tone_duration_max.Value);
fprintf('shock current  ');
fprintf(' %3.3f\n',app.shock_current.Value);

if app.shock_before_tone.Value
    fprintf('Shock Before Tone\n');
else
    fprintf('Tone Before Shock\n');
end

vl1 = str2double(app.tone1_freq.Value);
f1 = str2double(cell2mat(app.tone1_freq.Items(vl1)));
fprintf('Tone 1 Frequency     %d kHz \n ',f1);
vl2 = str2double(app.tone2_freq.Value);
f2 = str2double(cell2mat(app.tone2_freq.Items(vl2)));
fprintf('Tone 2 Frequency     %d kHz \n ',f2);
vl3 = str2double(app.tone3_freq.Value);
f3 = str2double(cell2mat(app.tone3_freq.Items(vl3)));
fprintf('Tone 3 Frequency     %d kHz \n ',f3);
vl4 = str2double(app.tone2_freq.Value);
f4 = str2double(cell2mat(app.tone4_freq.Items(vl4)));
fprintf('Tone 4 Frequency     %d kHz \n ',f4);
if app.tone1.Value
    if app.shock1.Value
        fprintf('Tone 1 Enabled with shock\n');
    else
        fprintf('Tone 1 Enabled with no shock\n');
    end
else
    fprintf('Tone 1 Disabled \n');
end

if app.tone2.Value
    if app.shock2.Value
        fprintf('Tone 2 Enabled with shock\n');
    else
        fprintf('Tone 2 Enabled with no shock\n');
    end
else
    fprintf('Tone 2 Disabled \n');
end

if app.tone3.Value
    if app.shock3.Value
        fprintf('Tone 3 Enabled with shock\n');
    else
        fprintf('Tone 3 Enabled with no shock\n');
    end
else
    fprintf('Tone 3 Disabled \n');
end

if app.tone4.Value
    if app.shock4.Value
        fprintf('Tone 4 Enabled with shock\n');
    else
        fprintf('Tone 4 Enabled with no shock\n');
    end
else
    fprintf('Tone 4 Disabled \n');
end


fprintf('tone volume ');
fprintf(' %d\n',app.volume.Value);
fprintf('initial delay ');
fprintf(' %3.3f\n',app.initial_delay.Value);

% start saving video  if selected
if app.save_video.Value
    vid.LoggingMode = 'disk';
%     diskLogger = VideoWriter('C:\matlab_code\shock_box\test.mj2', 'Archival');
%     diskLogger = VideoWriter('C:\matlab_code\shock_box\test.avi', 'Uncompressed AVI');
%     diskLogger = VideoWriter('C:\matlab_code\shock_box\test.mj2', 'Motion JPEG 2000');
    diskLogger = VideoWriter([fn  '.mp4' ], 'MPEG-4');
    vid.DiskLogger = diskLogger;
    triggerconfig(vid, 'immediate');
    pv = preview(vid);
    start(vid);
else
%     pv = preview(vid);
end

% wait until initial wait period expires
tic;
wait = 1;

while wait
    pause(0.05);
    drawnow;
    if toc > app.initial_delay.Value
        wait = 0;
    end
end

% set volume
vl = app.volume.Value;
fprintf(arduino,'%c',['v' vl]);

current_to_port = round(50*app.shock_current.Value);
number_of_cycles = app.number_of_cycles.Value;
shock_duration_min = app.shock_duration_min.Value;
shock_duration_max = app.shock_duration_max.Value;
interval_min = app.cycle_interval_min.Value;
interval_max = app.cycle_interval_max.Value;
tone_duration_min = app.tone_duration_min.Value;
tone_duration_max = app.tone_duration_max.Value;
delay_min = app.delay_min.Value;
delay_max = app.delay_max.Value;
shock_duration = shock_duration_min +  (shock_duration_max - shock_duration_min)*rand(number_of_cycles,1);  
interval = interval_min +  (interval_max - interval_min)*rand(number_of_cycles,1);  
sdelay  =  delay_min +  (delay_max - delay_min)*rand(number_of_cycles,1);  
tone_duration =  tone_duration_min +  (tone_duration_max - tone_duration_min)*rand(number_of_cycles,1);  

shock_duration = round(1000*shock_duration);
interval = round(1000*interval);
sdelay = round(1000*sdelay);
tone_duration = round(1000*tone_duration);


number_of_tones =  sum(app.tone1.Value + app.tone2.Value + app.tone3.Value + app.tone4.Value);

np = floor(number_of_cycles/number_of_tones);
number_of_cycles =  number_of_tones*np;


% sshock = zeros(number_of_cycles,1);
tn=  [];

if app.tone1.Value
    tm  = [str2double(app.tone1_freq.Value), app.shock1.Value];
    tm = repmat(tm,np,1);
    tn  = [tn ; tm];
end

if app.tone2.Value
    tm  = [str2double(app.tone2_freq.Value), app.shock2.Value];
    tm = repmat(tm,np,1);
    tn  = [tn ; tm];
end

if app.tone3.Value
    tm  = [str2double(app.tone3_freq.Value), app.shock3.Value];
    tm = repmat(tm,np,1);
    tn  = [tn ; tm];
end

if app.tone4.Value
    tm  = [str2double(app.tone4_freq.Value), app.shock4.Value];
    tm = repmat(tm,np,1);
    tn  = [tn ; tm];
end

% stn2 = [str2double(app.tone2_freq.Value) ,app.shock2.Value];
% stn3 = [str2double(app.tone3_freq.Value) ,app.shock3.Value];
% stn4 = [str2double(app.tone4_freq.Value) ,app.shock4.Value];
stone = tn(randperm(number_of_cycles),:);

app.STARTButton.Visible = 'off';
app.STOPButton.Visible = 'on';


% --------------------------
% print all values

fprintf('\n\n\tshock_dur\tdelay\ttone_freq\tshock\ttone_dur\tinterval\n\n');


for i = 1:number_of_cycles

if stone(i,2) == 0
    shock_duration(i) = 0;
end
    
fprintf('%d',i); 
fprintf('\t%d',shock_duration(i));    
fprintf('\t\t%d',sdelay(i));    
fprintf('\t%d kHz\t\t%d',str2double(cell2mat(app.tone1_freq.Items(stone(i,1)))),stone(i,2));    
fprintf('\t%d',tone_duration(i));    
fprintf('\t\t%d ',interval(i));    
fprintf('\n');
    
end
% --------------------------


bav = get(arduino,'BytesAvailable');

if bav > 0
    sdata = fread(arduino,bav);    
end

% tones off
 fprintf(arduino,'%c','b');


% --------------------------------------------------------------
% send number of cycles
ack = 0;
sdata = 0;
vl = app.tone_before_shock.Value;
ackstr = 'Stp';
fprintf(arduino,'%c','c');
   l = mod(number_of_cycles,256); 
   h = floor(number_of_cycles/256);
   fprintf(arduino,'%c',[l h vl current_to_port]);

    while ack == 0
    end
    ack = 0;
    sdata = 0;

% fprintf('number of cycles  = %d \n',number_of_cycles);

% --------------------------------------------------------------
% send shock durations
ackstr = 'aaa';
fprintf(arduino,'%c','d');
for i = 1:number_of_cycles
       l = mod(shock_duration(i),256); 
       h = floor(shock_duration(i)/256);
       fprintf(arduino,'%c',[l h ]);
end

while ack == 0
end
ack = 0;
sdata = 0;

% fprintf('Step received  durations  %d \n',i);
        
% --------------------------------------------------------------
% send cycle intervals
    
ackstr = 'bbb';
fprintf(arduino,'%c','e');
for i = 1:number_of_cycles
       temp = typecast(uint32(interval(i)),'uint8');
       fprintf(arduino,'%c',[char(temp(1))  char(temp(2))  char(temp(3))]);
end

while ack == 0
end
ack = 0;
sdata = 0;

% fprintf('Step received  intervals  %d \n',i);
        
% --------------------------------------------------------------
% send delays

ackstr = 'ccc';
fprintf(arduino,'%c','f');
for i = 1:number_of_cycles
       l = mod(sdelay(i),256); 
       h = floor(sdelay(i)/256);
       fprintf(arduino,'%c',[l h ]);
end

while ack == 0
end
ack = 0;
sdata = 0;

% fprintf('Step received delays   %d \n',i);

% --------------------------------------------------------------
% send  tone durations

ackstr = 'ddd';
fprintf(arduino,'%c','g');
for i = 1:number_of_cycles
       l = mod(tone_duration(i),256); 
       h = floor(tone_duration(i)/256);
       fprintf(arduino,'%c',[l h ]);
end

while ack == 0
end
ack = 0;
sdata = 0;
% fprintf('Step received  tone durations %d \n',i);


% --------------------------------------------------------------
% send  tones

ackstr = 'eee';
fprintf(arduino,'%c','h');
for i = 1:number_of_cycles
       l = stone(i); 
       fprintf(arduino,'%c',l);
end

while ack == 0
end
ack = 0;
sdata = 0;
 fprintf('Step received  tones %d \n',i);
% start process
ackstr = 'fff';
fprintf(arduino,'%c','y');
tic;
strt = toc;
% wait for test to finish
ncs = number_of_cycles;
app.ncs_left.Text = num2str(ncs);
idx = 1;
ival = interval(idx)/1000;

while ack == 0
    if ncl
        ncs = ncs - 1;
        app.ncs_left.Text = num2str(ncs);
        ncl = 0;
        strt  = toc;
        ival = interval(idx)/1000;
        idx = idx + 1;
    end
        app.time_to_next_cycle.Text = num2str(ival - (toc-strt),'%3.1f');
    
    pause(0.001);
end

if app.save_video.Value
    stoppreview(vid);
    stop(vid);
end

diary off;
app.STARTButton.Visible = 'on';
app.STOPButton.Visible = 'off';
