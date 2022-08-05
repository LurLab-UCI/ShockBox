     
function loadStatus(app,fn)
%                return
%             load('MyAppDefaultValues.mat','state');
    load(fn);

    h = findobj(app.UIFigure,'Type','uicheckbox');
    for i = 1:length(h)
        try
               h(i).Value = state.checkboxes(i);
        catch
            break;
        end
    end

     h = findobj(app.UIFigure,'Type','uieditfield');
     for i = 1:length(h)
         try
            h(i).Value = state.editfields{i};
         catch
             break;
         end
     end

     h = findobj(app.UIFigure,'Type','uinumericeditfield');
     for i = 1:length(h)
         try
            h(i).Value = state.uinumericeditfields{i};
         catch
             break;
         end
     end


     h = findobj(app.UIFigure,'Type','uiradiobutton');
      for i = 1:length(h)
          try
            h(i).Value = state.uiradiobuttons{i};
          catch
              break;
          end
      end

     h = findobj(app.UIFigure,'Type','uiDropDown');
      for i = 1:length(h)
          try
            h(i).Value = state.dropdowns{i};
          catch
              break
          end
      end

     app.UIFigure.Position = ps;

end        
        