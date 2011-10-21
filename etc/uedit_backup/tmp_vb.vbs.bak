Dim oWord, oDocument, oSelection

' Constants
Const wdNormalView = 1
Const wdOutlineView = 2
Const wdPrintView = 4

Const wdSeekMainDocument = 0
Const wdSeekCurrentPageHeader = 9

Const wdLine = 5
Const wdStory = 6
Const wdCell = 12

Const wdPaneNone = 0

Const wdWord9TableBehavior = 1

Const wdAutoFitContent = 1

Const wdToggle = 9999998

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
oSelection.TypeText("TLFCOPY: ")
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
oWord.ActiveDocument.Tables.Add (oSelection.Range,108,6, wdWord9TableBehavior, wdAutoFitContent)

If oSelection.Tables(1).Style <> "Table Grid" Then
        oSelection.Tables(1).Style = "Table Grid"
End If
oSelection.Tables(1).ApplyStyleHeadingRows = True
oSelection.Tables(1).ApplyStyleLastRow = True
oSelection.Tables(1).ApplyStyleFirstColumn = True
oSelection.Tables(1).ApplyStyleLastColumn = True
oSelection.HomeKey (wdStory)
oSelection.Font.Bold = wdToggle
oSelection.TypeText ("Date")
oSelection.MoveRight (wdCell)
oSelection.Font.Bold = wdToggle
oSelection.TypeText ("Status")
oSelection.MoveRight (wdCell)
oSelection.Font.Bold = wdToggle
oSelection.TypeText ("Txn Type")
oSelection.MoveRight (wdCell)
oSelection.Font.Bold = wdToggle
oSelection.TypeText ("ABM")
oSelection.MoveRight (wdCell)
oSelection.Font.Bold = wdToggle
oSelection.TypeText ("Seq #")
oSelection.MoveRight (wdCell)
oSelection.Font.Bold = wdToggle
oSelection.TypeText ("More Info")
oSelection.MoveDown (wdCell)
oSelection.HomeKey (wdStory)
oSelection.TypeText (" 14:51:03.46&nbsp;[2]")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("<b>ABM&nbsp;&rarr;&nbsp;ICTS</b>")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("Deposit&nbsp;Rq (0200)")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("9983")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("1466")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("Card: #1086, Amt: $2120.96, SDA")
oSelection.MoveDown (wdLine)
oSelection.HomeKey (wdStory)
oSelection.TypeText (" 14:51:06.23&nbsp;[2]")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("<b>ICTS&nbsp;&rarr;&nbsp;ABM</b>")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("Deposit&nbsp;Rs (0210)")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("9983")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("1466")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("ISO=19, B24=68, IFX=6340</b>/A portion of the requested deposit amount was held by the authorizer.  [Amt: $2120.96, Card: #1086, SDA, CreditHeld: $2120.96]")
oSelection.MoveDown (wdLine)
oSelection.HomeKey (wdStory)
oSelection.TypeText (" 14:51:12.45&nbsp;[4]")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("<b>ICTS Down</b><br>ABM&nbsp;&rarr;&nbsp;AUTH<br>(via HISO)")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("Dep&nbsp;Authorize&nbsp;Rq (0200)")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("9983")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("1466")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("Card: #1086")
oSelection.MoveDown (wdLine)
oSelection.HomeKey (wdStory)
oSelection.TypeText (" 14:51:16.87&nbsp;[3]")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("<b>B24 Standin</b><br>AUTH&nbsp;&rarr;&nbsp;ABM")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("Dep&nbsp;Authorize&nbsp;Rs (0210)")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("9983")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("1466")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("B24=56, IFX=2300</b>/Single or Source Account Invalid: {0}  [CreditHeld: $0.00]")
oSelection.MoveDown (wdLine)
oSelection.HomeKey (wdStory)
oSelection.TypeText (" 14:51:16.89&nbsp;[0]")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("<b>DH&nbsp;&rarr;&nbsp;ICTS</b>")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("Deposit&nbsp;Completion&nbsp;Rq (0220)")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("9983")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("1466")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("Card: #1086 0220/20 (Dep) ABM=9983 B24-RC=56 SEQ#=1466")
oSelection.MoveDown (wdLine)
oSelection.HomeKey (wdStory)
oSelection.TypeText (" 14:51:17.27&nbsp;[0]")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("<b>ICTS&nbsp;&rarr;&nbsp;HISO</b>")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("Deposit&nbsp;Completion&nbsp;Rs (0230)")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("9983")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("1466")
oSelection.MoveRight (wdCell)
oSelection.TypeText ("ISO=00  [Card: #1086 0230/21 (Dep) ABM=9983 ISO-RC=0 SEQ#=1466]")
oSelection.MoveDown (wdLine)
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

' Set header to file name of document
If oWord.ActiveWindow.View.SplitSpecial <> wdPaneNone Then
    oWord.ActiveWindow.Panes(2).Close
End If
If oWord.ActiveWindow.ActivePane.View.Type = wdNormalView Or _
        oWord.ActiveWindow.ActivePane.View.Type = wdOutlineView Then
    oWord.ActiveWindow.ActivePane.View.Type = wdPrintView
End If
oWord.ActiveDocument.ActiveWindow.ActivePane.View.SeekView = wdSeekCurrentPageHeader
oWord.NormalTemplate.AutoTextEntries("Filename").Insert(oSelection.Range)
oWord.ActiveDocument.ActiveWindow.ActivePane.View.SeekView = 0
