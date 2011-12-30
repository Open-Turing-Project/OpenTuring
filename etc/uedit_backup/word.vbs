Dim oWord, oDocument, oSelection

Set oWord = CreateObject ("Word.Application")

oWord.Visible = true
oDocument = oWord.Documents.Add()
oWord.ActiveDocument.PageSetup.LineNumbering.Active = False
oWord.ActiveDocument.PageSetup.Orientation = wdOrientPortrait
oWord.ActiveDocument.PageSetup.TopMargin = 72
oWord.ActiveDocument.PageSetup.BottomMargin = 72
oWord.ActiveDocument.PageSetup.LeftMargin = 72
oWord.ActiveDocument.PageSetup.RightMargin = 72

Set oSelection = oWord.Selection
oSelection.Font.Bold = true
oSelection.Font.Name = "Courier New"
oSelection.Font.Size = 11
oSelection.Font.Underline = true
oSelection.TypeText("Evaluation Summary")
oSelection.Font.Underline = false
oSelection.Font.Bold = false
oSelection.Font.Size = 9
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 18
oSelection.TypeText(chr(13))
oSelection.TypeText("No issues were found when inspected from the Base24 viewpoint.")
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 0
oSelection.Font.Bold = true
oSelection.TypeText("TLFCOPY:")
oSelection.Font.Bold = false
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 18
oSelection.TypeText("                                                            Log          Resp")
oSelection.TypeText(chr(13))
oSelection.TypeText("Rec #  Type ACQ  ISS        Card #        Rsp Amount  Date  Time  Seq #  Time")
oSelection.TypeText(chr(13))
oSelection.TypeText("------ ---- ---- ---- ------------------- --- ------- ---- ------ ------ ----")
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 0
oSelection.Font.Bold = true
oSelection.TypeText("ILFCOPY:")
oSelection.Font.Bold = false
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 18
oSelection.TypeText(chr(13))
oSelection.TypeText("RECNUM TYPE PAN                 RSP TD  AMOUNT        DATE   TIME   TRACE  SUB")
oSelection.TypeText(chr(13))
oSelection.TypeText("------ ---- ------------------- --- --- ------------- ------ ------ ------ ---")
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.TypeText("RECNUM TYPE TRACK2 AT BYTE 1    RSP CRDFI  TERMFI DATE   TIME   SEQ #  RVSL")
oSelection.TypeText(chr(13))
oSelection.TypeText("------ ---- ------------------- --- ------ ------ ------ ------ ------ ----")
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 0
oSelection.Font.Bold = true
oSelection.TypeText("AUDIT 1:")
oSelection.Font.Bold = false
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 18
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 0
oSelection.Font.Bold = true
oSelection.TypeText("AUDIT 2:")
oSelection.Font.Bold = false
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 18
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 0
oSelection.Font.Bold = true
oSelection.TypeText("EMS:")
oSelection.Font.Bold = false
oSelection.TypeText(chr(13))
oSelection.ParagraphFormat.LeftIndent = 18
oSelection.TypeText(chr(13))
oSelection.TypeText("No relevant EMS messages")
oSelection.TypeText(chr(13))
If oWord.ActiveWindow.ActivePane.View.Type = wdNormalView Or _
		oWord.ActiveWindow.ActivePane.View.Type = wdOutlineView Then
	oWord.ActiveWindow.ActivePane.View.Type = wdPrintView
End If
oWord.ActiveDocument.ActiveWindow.ActivePane.View.SeekView = 9
oWord.NormalTemplate.AutoTextEntries("Filename").Insert(oSelection.Range)
oWord.ActiveDocument.ActiveWindow.ActivePane.View.SeekView = 0
