unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% LabelClass - Class that implements text items much as they appear in
%              MS Windows 3.1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of LabelClass is as follows:
%   procedure Show - Display the widget.
%   procedure Hide - Hide the widget.
%   procedure Dispose - Hide the widget and free any data structures allocated
%                       by the class.  To be called before the Widget is freed.
%   procedure SetPosition (x, y : int) - Move the widget to (x, y).
%   procedure SetSize (width, height : int) - Resize the widget.
%   procedure SetPositionAndSize (x, y, width, height : int) - Move and resize
%                                                              the widget.
%   procedure Initialize (x, y : int, text : string)
%                       x, y - The lower left corner of the text.
%                       text - The text in the label.
%   procedure InitializeFull (x, y : int, text : string,
%                             width, height, alignment, fontID : int)
%                       x, y - The lower left corner of the text.
%                       text - The text in the label.
%                       width, height - The width and height of the area the
%                                       text is to be placed in.
%                       alignment - The alignment of the text in the box.
%                       fontID - The font ID of the text to be used.
%   procedure SetLabel (text : string) - Set label to text.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of LabelClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure ComputeWidgetPosition - Compute the location of the label, etc.
%   procedure DrawText (clr : int) - Draws the label in specified colour.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of LabelClass are as follows:
%   originalX, originalY - The x, y values specified by the user.
%   originalWidth, originalHeight - The width, height values specified by the
%                                   user.
%   x, y - The location of the lower left corner of the widget.
%   width, height - The width and height of the widget.
%   showing - Is the widget visible.
%   initialized - Has the widget been initialized.
%   drawn - The widget has been drawn at least once.  Used to calculate the
%           position for drawing.
%   window - The window the widget is in.
%   text - The text of the label.
%   textFontID - The font ID that the label is drawn in.
%   alignment - The alignment of the text in the box.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of LabelClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class LabelClass
    inherit GenericWidgetBodyClass

    import WidgetGlobals

    %
    % Exported routines for LabelClass
    %
    export
    % The external interface
	Initialize, InitializeFull, SetLabel


    %
    % The inheritable variables of LabelClass
    %
    var text : string % The text of the label
    var textFontID : int % The font ID that the label is drawn in
    var alignment : int % The alignment of the text in the box

    %
    % The inheritable subprograms of LabelClass
    %
    procedure DrawText (clr : int)
	Font.Draw (text, x, y, textFontID, clr)
    end DrawText


    %
    % The external subprograms of LabelClass
    %
    procedure InitializeFull (newX, newY : int, newText : string,
	    newWidth, newHeight, newAlignment, newFontID : int)
	SetPositionAndSize (newX, newY, newWidth, newHeight)
	text := newText
	alignment := newAlignment
	case alignment and WidgetGlobals.HORIZONTAL_ALIGNMENTS of
	    label WidgetGlobals.LEFT, WidgetGlobals.CENTRE,
		    WidgetGlobals.RIGHT :
	    label 0 :
		alignment += WidgetGlobals.LEFT
	    label :
		AssertFailed ("Illegal Horizontal Alignment")
	end case
	case alignment and WidgetGlobals.VERTICAL_ALIGNMENTS of
	    label WidgetGlobals.TOP, WidgetGlobals.MIDDLE,
		    WidgetGlobals.BOTTOM :
	    label 0 :
		alignment += WidgetGlobals.BOTTOM
	    label :
		AssertFailed ("Illegal Vertical Alignment")
	end case
	if (alignment and ~WidgetGlobals.ALL_ALIGNMENTS) not= 0 then
	    AssertFailed ("Illegal Text Alignment");
	end if
	textFontID := TranslateFont (newFontID)

	GeneralInitialize
    end InitializeFull


    procedure Initialize (newX, newY : int, newText : string)
	InitializeFull (newX, newY, newText, 0, 0, 0, 0)
    end Initialize


    procedure SetLabel (newText : string)
	if showing then
	    EraseWidget
	    text := newText
	    drawn := false
	    DrawWidget
	else
	    text := newText
	    drawn := false
	end if
    end SetLabel


    %
    % Overridden subprograms
    %
    body procedure ComputeWidgetPosition
	var textWidth, textHeight, descent, ascent, leading : int
	textWidth := Font.Width (text, textFontID)
	Font.Sizes (textFontID, textHeight, ascent, descent, leading)
	textHeight := descent + ascent - leading
	#if _DOS_ then
	    textHeight -= 3
	#end if
	if (alignment and WidgetGlobals.RIGHT) not= 0 then
	    x := originalX + originalWidth - textWidth
	elsif (alignment and WidgetGlobals.CENTRE) not= 0 then
	    x := originalX + (originalWidth - textWidth) div 2
	else
	    x := originalX
	end if
	if (alignment and WidgetGlobals.TOP) not= 0 then
	    y := originalY + originalHeight - ascent
	elsif (alignment and WidgetGlobals.MIDDLE) not= 0 then
	    y := originalY + (originalHeight - textHeight) div 2
	else
	    y := originalY + descent
	end if
	width := textWidth
	height := textHeight
	drawn := true
    end ComputeWidgetPosition


    body procedure DrawWidget
	if not drawn then
	    ComputeWidgetPosition
	end if
	DrawText (foregroundColour)
    end DrawWidget


    body procedure EraseWidget
	if not drawn then
	    ComputeWidgetPosition
	end if
	DrawText (backgroundColour)
    end EraseWidget
end LabelClass

