  function saveStatus(app,fn)
            
    h = findobj(app.UIFigure,'Type','uicheckbox');
    xarr = zeros(length(h),1);
    xa = [];
    for i = 1:length(h)
       xarr(i) = h(i).Value;
    end
    if ~isempty(xarr)
        state.checkboxes = xarr;
    end

     h = findobj(app.UIFigure,'Type','uieditfield');
     for i = 1:length(h)
        xa{i} = h(i).Value;
     end
    if ~isempty(xa)
        state.editfields = xa;
    end

     h = findobj(app.UIFigure,'Type','uinumericeditfield');
     for i = 1:length(h)
        xa{i} = h(i).Value;
     end
    if ~isempty(xa)
         state.uinumericeditfields = xa;
    end

     h = findobj(app.UIFigure,'Type','uiradiobutton');
     for i = 1:length(h)
        xa{i} = h(i).Value;
     end
    if ~isempty(xa)
        state.uiradiobuttons = xa;
    end

     h = findobj(app.UIFigure,'Type','uiDropDown');
     for i = 1:length(h)
        xa{i} = h(i).Value;
     end
    if ~isempty(xa)
        state.dropdowns = xa;
    end
     

   ps = app.UIFigure.Position;
   save(fn,'state','ps');

  end