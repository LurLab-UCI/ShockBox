function read_serial(obj,~)
global sdata ack ackstr ncl;

bav = get(obj,'BytesAvailable');
if bav > 0
    sdata = fread(obj,bav);
%      fprintf('%4.3f   %d   %s  %s \n',toc,bav,sdata,ackstr);   %GL edit added \n to string
%      fprintf('%4.3f   %d   %s  \n',toc,bav,sdata);   %GL edit added \n to string
     if contains(char(sdata'),ackstr)
         ack = 1;
     end
     if contains(char(sdata'),'ncl')
         ncl = 1;
     end
end