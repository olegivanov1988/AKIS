object Product: TProduct
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1069#1082#1088#1072#1085' '#1087#1088#1086#1074#1077#1088#1082#1080' '#1048#1079#1076#1077#1083#1080#1103
  ClientHeight = 1080
  ClientWidth = 1920
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  WindowState = wsMaximized
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Shape5: TShape
    Left = 1083
    Top = 837
    Width = 125
    Height = 4
    Brush.Color = clBlack
  end
  object sBevel1: TsBevel
    Left = 442
    Top = 827
    Width = 130
    Height = 129
  end
  object Shape1: TShape
    Left = 871
    Top = 969
    Width = 68
    Height = 2
    Brush.Color = clBlack
  end
  object Shape10: TShape
    Left = 870
    Top = 918
    Width = 2
    Height = 53
    Brush.Color = clBlack
  end
  object Shape3: TShape
    Left = 571
    Top = 864
    Width = 367
    Height = 2
    Brush.Color = clBlack
  end
  object Shape4: TShape
    Left = 673
    Top = 811
    Width = 265
    Height = 2
    Brush.Color = clBlack
  end
  object Shape6: TShape
    Left = 290
    Top = 500
    Width = 2
    Height = 236
    Brush.Color = clBlack
  end
  object Shape7: TShape
    Left = 220
    Top = 577
    Width = 71
    Height = 2
    Brush.Color = clBlack
  end
  object Shape8: TShape
    Left = 291
    Top = 734
    Width = 384
    Height = 2
    Brush.Color = clBlack
  end
  object Shape9: TShape
    Left = 673
    Top = 734
    Width = 2
    Height = 77
    Brush.Color = clBlack
  end
  object Shape11: TShape
    Left = 898
    Top = 500
    Width = 2
    Height = 258
    Brush.Color = clBlack
  end
  object Shape12: TShape
    Left = 827
    Top = 577
    Width = 73
    Height = 2
    Brush.Color = clBlack
  end
  object Shape13: TShape
    Left = 898
    Top = 757
    Width = 41
    Height = 2
    Brush.Color = clBlack
  end
  object Shape14: TShape
    Left = 1083
    Top = 705
    Width = 417
    Height = 2
    Brush.Color = clBlack
  end
  object Shape15: TShape
    Left = 1498
    Top = 668
    Width = 2
    Height = 39
    Brush.Color = clBlack
  end
  object Shape16: TShape
    Left = 1226
    Top = 668
    Width = 2
    Height = 39
    Brush.Color = clBlack
  end
  object Shape17: TShape
    Left = 572
    Top = 918
    Width = 367
    Height = 2
    Brush.Color = clBlack
  end
  object sLabel15: TsLabel
    Left = 801
    Top = 35
    Width = 318
    Height = 29
    Caption = #1069#1082#1088#1072#1085' '#1087#1088#1086#1074#1077#1088#1082#1080' '#1048#1079#1076#1077#1083#1080#1103
    ParentFont = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 6833674
    Font.Height = -24
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    UseSkinColor = False
  end
  object iScope1: TiScope
    Left = 91
    Top = 140
    Width = 400
    Height = 330
    SamplesPerSecond = 10000
    AutoFrameRate = False
    UpdateFrameRate = 2
    HideControlPanels = True
    PrintMarginLeft = 1.000000000000000000
    PrintMarginTop = 1.000000000000000000
    PrintMarginRight = 1.000000000000000000
    PrintMarginBottom = 1.000000000000000000
    PrintDocumentName = 'Untitled'
    Channels = <
      item
        VoltsPerDivision = 5.000000000000000000
        Coupling = isccDC
        TitleText = 'CH1'
        Color = clLime
        TraceLineWidth = 2
        RefLineShow = False
      end>
    Display = <
      item
      end>
    TimeBase = <
      item
        SecPerDiv = 0.001000000000000000
      end>
    Trigger = <
      item
        Level = 50.000000000000000000
        PositiveSlope = True
        SourceIndex = 0
        Auto = True
      end>
  end
  object sPanel32: TsPanel
    Left = 91
    Top = 469
    Width = 400
    Height = 31
    TabOrder = 1
    SkinData.SkinSection = 'TOOLBAR'
    object sListBox5: TsListBox
      Left = 192
      Top = 5
      Width = 57
      Height = 20
      Color = 15921906
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.Strings = (
        '50 '#1042
        '20 '#1042
        '10 '#1042
        '5 '#1042
        '2 '#1042
        '1 '#1042
        '500 '#1084#1042
        '200 '#1084#1042
        '100 '#1084#1042
        '50 '#1084#1042
        '20 '#1084#1042
        '10 '#1084#1042
        '5 '#1084#1042
        '2 '#1084#1042
        '1 '#1084#1042
        '100 '#1084#1082#1042
        '50 '#1084#1082#1042
        '20 '#1084#1082#1042
        '10 '#1084#1082#1042
        '5 '#1084#1082#1042
        '2 '#1084#1082#1042
        '1 '#1084#1082#1042)
      ParentFont = False
      TabOrder = 0
      BoundLabel.Active = True
      BoundLabel.Caption = 'Volts/DIV'
      BoundLabel.Indent = 0
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = 16773855
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclLeft
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
    object sListBox6: TsListBox
      Left = 312
      Top = 5
      Width = 57
      Height = 20
      Color = 15921906
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.Strings = (
        'DC'
        'AC')
      ParentFont = False
      TabOrder = 1
      BoundLabel.Active = True
      BoundLabel.Caption = 'Coupling'
      BoundLabel.Indent = 0
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = 16773855
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclLeft
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
    object sListBox4: TsListBox
      Left = 71
      Top = 5
      Width = 57
      Height = 20
      Color = 15921906
      ExtendedSelect = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.Strings = (
        '0,1 '#1089
        '50 '#1084#1089
        '20 '#1084#1089
        '10 '#1084#1089
        '5 '#1084#1089
        '2 '#1084#1089
        '1 '#1084#1089
        '0,5 '#1084#1089
        '0,2 '#1084#1089
        '0,1 '#1084#1089
        '50 '#1084#1082#1089
        '20 '#1084#1082#1089
        '10 '#1084#1082#1089
        '5 '#1084#1082#1089
        '2 '#1084#1082#1089
        '1 '#1084#1082#1089
        '0,5 '#1084#1082#1089
        '0,2 '#1084#1082#1089
        '0,1 '#1084#1082#1089)
      ParentFont = False
      ScrollWidth = -1
      TabOrder = 2
      BoundLabel.Active = True
      BoundLabel.Caption = 'SEC/DIV'
      BoundLabel.Indent = 0
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = 16773855
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclLeft
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
  end
  object iScope2: TiScope
    Left = 700
    Top = 140
    Width = 400
    Height = 330
    SamplesPerSecond = 10000
    AutoFrameRate = False
    UpdateFrameRate = 2
    HideControlPanels = True
    PrintMarginLeft = 1.000000000000000000
    PrintMarginTop = 1.000000000000000000
    PrintMarginRight = 1.000000000000000000
    PrintMarginBottom = 1.000000000000000000
    PrintDocumentName = 'Untitled'
    Channels = <
      item
        VoltsPerDivision = 5.000000000000000000
        Coupling = isccDC
        TitleText = 'CH1'
        Color = clLime
        TraceLineWidth = 2
        RefLineShow = False
      end>
    Display = <
      item
      end>
    TimeBase = <
      item
        SecPerDiv = 0.001000000000000000
      end>
    Trigger = <
      item
        Level = 50.000000000000000000
        PositiveSlope = True
        SourceIndex = 0
        Auto = True
      end>
  end
  object sPanel1: TsPanel
    Left = 700
    Top = 469
    Width = 400
    Height = 31
    TabOrder = 3
    SkinData.SkinSection = 'TOOLBAR'
    object sListBox1: TsListBox
      Left = 192
      Top = 5
      Width = 57
      Height = 20
      Color = 15921906
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.Strings = (
        '50 '#1042
        '20 '#1042
        '10 '#1042
        '5 '#1042
        '2 '#1042
        '1 '#1042
        '500 '#1084#1042
        '200 '#1084#1042
        '100 '#1084#1042
        '50 '#1084#1042
        '20 '#1084#1042
        '10 '#1084#1042
        '5 '#1084#1042
        '2 '#1084#1042
        '1 '#1084#1042
        '100 '#1084#1082#1042
        '50 '#1084#1082#1042
        '20 '#1084#1082#1042
        '10 '#1084#1082#1042
        '5 '#1084#1082#1042
        '2 '#1084#1082#1042
        '1 '#1084#1082#1042)
      ParentFont = False
      TabOrder = 0
      BoundLabel.Active = True
      BoundLabel.Caption = 'Volts/DIV'
      BoundLabel.Indent = 0
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = 16773855
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclLeft
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
    object sListBox2: TsListBox
      Left = 312
      Top = 5
      Width = 57
      Height = 20
      Color = 15921906
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.Strings = (
        'DC'
        'AC')
      ParentFont = False
      TabOrder = 1
      BoundLabel.Active = True
      BoundLabel.Caption = 'Coupling'
      BoundLabel.Indent = 0
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = 16773855
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclLeft
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
    object sListBox3: TsListBox
      Left = 71
      Top = 5
      Width = 57
      Height = 20
      Color = 15921906
      ExtendedSelect = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.Strings = (
        '0,1 '#1089
        '50 '#1084#1089
        '20 '#1084#1089
        '10 '#1084#1089
        '5 '#1084#1089
        '2 '#1084#1089
        '1 '#1084#1089
        '0,5 '#1084#1089
        '0,2 '#1084#1089
        '0,1 '#1084#1089
        '50 '#1084#1082#1089
        '20 '#1084#1082#1089
        '10 '#1084#1082#1089
        '5 '#1084#1082#1089
        '2 '#1084#1082#1089
        '1 '#1084#1082#1089
        '0,5 '#1084#1082#1089
        '0,2 '#1084#1082#1089
        '0,1 '#1084#1082#1089)
      ParentFont = False
      ScrollWidth = -1
      TabOrder = 2
      BoundLabel.Active = True
      BoundLabel.Caption = 'SEC/DIV'
      BoundLabel.Indent = 0
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = 16773855
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclLeft
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
  end
  object iScope3: TiScope
    Left = 1299
    Top = 308
    Width = 400
    Height = 330
    SamplesPerSecond = 10000
    AutoFrameRate = False
    UpdateFrameRate = 2
    HideControlPanels = True
    PrintMarginLeft = 1.000000000000000000
    PrintMarginTop = 1.000000000000000000
    PrintMarginRight = 1.000000000000000000
    PrintMarginBottom = 1.000000000000000000
    PrintDocumentName = 'Untitled'
    Channels = <
      item
        VoltsPerDivision = 5.000000000000000000
        Coupling = isccDC
        TitleText = 'CH1'
        Color = clLime
        TraceLineWidth = 2
        RefLineShow = False
      end>
    Display = <
      item
      end>
    TimeBase = <
      item
        SecPerDiv = 0.001000000000000000
      end>
    Trigger = <
      item
        Level = 50.000000000000000000
        PositiveSlope = True
        SourceIndex = 0
        Auto = True
      end>
  end
  object sPanel2: TsPanel
    Left = 1299
    Top = 637
    Width = 400
    Height = 31
    TabOrder = 5
    SkinData.SkinSection = 'TOOLBAR'
    object sListBox7: TsListBox
      Left = 192
      Top = 5
      Width = 57
      Height = 20
      Color = 15921906
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.Strings = (
        '50 '#1042
        '20 '#1042
        '10 '#1042
        '5 '#1042
        '2 '#1042
        '1 '#1042
        '500 '#1084#1042
        '200 '#1084#1042
        '100 '#1084#1042
        '50 '#1084#1042
        '20 '#1084#1042
        '10 '#1084#1042
        '5 '#1084#1042
        '2 '#1084#1042
        '1 '#1084#1042
        '100 '#1084#1082#1042
        '50 '#1084#1082#1042
        '20 '#1084#1082#1042
        '10 '#1084#1082#1042
        '5 '#1084#1082#1042
        '2 '#1084#1082#1042
        '1 '#1084#1082#1042)
      ParentFont = False
      TabOrder = 0
      BoundLabel.Active = True
      BoundLabel.Caption = 'Volts/DIV'
      BoundLabel.Indent = 0
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = 16773855
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclLeft
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
    object sListBox8: TsListBox
      Left = 312
      Top = 5
      Width = 57
      Height = 20
      Color = 15921906
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.Strings = (
        'DC'
        'AC')
      ParentFont = False
      TabOrder = 1
      BoundLabel.Active = True
      BoundLabel.Caption = 'Coupling'
      BoundLabel.Indent = 0
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = 16773855
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclLeft
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
    object sListBox9: TsListBox
      Left = 71
      Top = 5
      Width = 57
      Height = 20
      Color = 15921906
      ExtendedSelect = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.Strings = (
        '0,1 '#1089
        '50 '#1084#1089
        '20 '#1084#1089
        '10 '#1084#1089
        '5 '#1084#1089
        '2 '#1084#1089
        '1 '#1084#1089
        '0,5 '#1084#1089
        '0,2 '#1084#1089
        '0,1 '#1084#1089
        '50 '#1084#1082#1089
        '20 '#1084#1082#1089
        '10 '#1084#1082#1089
        '5 '#1084#1082#1089
        '2 '#1084#1082#1089
        '1 '#1084#1082#1089
        '0,5 '#1084#1082#1089
        '0,2 '#1084#1082#1089
        '0,1 '#1084#1082#1089)
      ParentFont = False
      ScrollWidth = -1
      TabOrder = 2
      BoundLabel.Active = True
      BoundLabel.Caption = 'SEC/DIV'
      BoundLabel.Indent = 0
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = 16773855
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclLeft
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
  end
  object sPanel8: TsPanel
    Left = 1208
    Top = 743
    Width = 517
    Height = 193
    Align = alCustom
    BevelOuter = bvLowered
    Caption = #1048#1079#1076#1077#1083#1080#1077
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
    SkinData.SkinSection = 'PANEL_LOW'
  end
  object sPanel3: TsPanel
    Left = 1573
    Top = 188
    Width = 185
    Height = 100
    BevelOuter = bvLowered
    TabOrder = 7
    SkinData.SkinSection = 'PANEL_LOW'
    object sButton5: TsButton
      Left = 8
      Top = 7
      Width = 169
      Height = 46
      Caption = #1047#1072#1074#1077#1088#1096#1080#1090#1100' '#1087#1088#1086#1074#1077#1088#1082#1091#13#10#1086#1090#1082#1083#1102#1095#1080#1090#1100' '#1087#1080#1090#1072#1085#1080#1077
      Enabled = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clLime
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      SkinData.CustomFont = True
      SkinData.SkinSection = 'BUTTON'
    end
    object sButton6: TsButton
      Left = 8
      Top = 59
      Width = 169
      Height = 33
      Caption = #1055#1086#1074#1090#1086#1088#1080#1090#1100' '#1087#1088#1086#1074#1077#1088#1082#1091
      Enabled = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      SkinData.SkinSection = 'BUTTON'
    end
  end
  object sPanel4: TsPanel
    Left = 1573
    Top = 148
    Width = 185
    Height = 41
    BevelOuter = bvLowered
    Caption = #1055#1088#1086#1074#1077#1088#1082#1072' '#1073#1083#1086#1082#1072
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 8
    SkinData.CustomFont = True
    SkinData.SkinSection = 'PANEL_LOW'
  end
  object sPanel5: TsPanel
    Left = 1573
    Top = 95
    Width = 185
    Height = 54
    BevelOuter = bvLowered
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 9
    SkinData.CustomFont = True
    SkinData.SkinSection = 'PANEL_LOW'
    object sButton2: TsButton
      Left = 8
      Top = 12
      Width = 169
      Height = 33
      Caption = #1047#1072#1087#1091#1089#1082' '#1087#1080#1090#1072#1085#1080#1103
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      SkinData.SkinSection = 'BUTTON'
    end
  end
  object sButton1: TsButton
    Left = 468
    Top = 842
    Width = 75
    Height = 42
    Caption = #1057#1080#1075#1085#1072#1083' '#1057#1050'-'#1056#13#10#1047#1040#1055#1059#1057#1050
    TabOrder = 10
    SkinData.SkinSection = 'BUTTON'
  end
  object sButton3: TsButton
    Left = 469
    Top = 899
    Width = 75
    Height = 42
    Caption = #1057#1080#1075#1085#1072#1083' '#1052#1050'-'#1056#13#10#1047#1040#1055#1059#1057#1050
    TabOrder = 11
    SkinData.SkinSection = 'BUTTON'
  end
  object sPanel21: TsPanel
    Left = 104
    Top = 593
    Width = 115
    Height = 30
    BevelEdges = []
    BevelOuter = bvNone
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clLime
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 12
    SkinData.CustomColor = True
    SkinData.CustomFont = True
    SkinData.SkinSection = 'CHECKBOX'
  end
  object sPanel22: TsPanel
    Left = 104
    Top = 563
    Width = 115
    Height = 30
    BevelEdges = [beBottom]
    BevelKind = bkFlat
    BevelOuter = bvNone
    Caption = 'T ='
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clLime
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 13
    SkinData.CustomColor = True
    SkinData.CustomFont = True
    SkinData.SkinSection = 'CHECKBOX'
  end
  object sPanel6: TsPanel
    Left = 104
    Top = 533
    Width = 115
    Height = 30
    BevelEdges = [beBottom]
    BevelKind = bkFlat
    BevelOuter = bvNone
    Caption = 'U ='
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clLime
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 14
    SkinData.CustomColor = True
    SkinData.CustomFont = True
    SkinData.SkinSection = 'CHECKBOX'
  end
  object sPanel7: TsPanel
    Left = 717
    Top = 593
    Width = 115
    Height = 30
    BevelEdges = []
    BevelOuter = bvNone
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clLime
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 15
    SkinData.CustomColor = True
    SkinData.CustomFont = True
    SkinData.SkinSection = 'CHECKBOX'
  end
  object sPanel9: TsPanel
    Left = 717
    Top = 563
    Width = 115
    Height = 30
    BevelEdges = [beBottom]
    BevelKind = bkFlat
    BevelOuter = bvNone
    Caption = 'T ='
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clLime
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 16
    SkinData.CustomColor = True
    SkinData.CustomFont = True
    SkinData.SkinSection = 'CHECKBOX'
  end
  object sPanel10: TsPanel
    Left = 717
    Top = 533
    Width = 115
    Height = 30
    BevelEdges = [beBottom]
    BevelKind = bkFlat
    BevelOuter = bvNone
    Caption = 'U ='
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clLime
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 17
    SkinData.CustomColor = True
    SkinData.CustomFont = True
    SkinData.SkinSection = 'CHECKBOX'
  end
  object sPanel11: TsPanel
    Left = 1170
    Top = 638
    Width = 115
    Height = 30
    BevelEdges = []
    BevelOuter = bvNone
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clLime
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 18
    SkinData.CustomColor = True
    SkinData.CustomFont = True
    SkinData.SkinSection = 'CHECKBOX'
  end
  object sPanel12: TsPanel
    Left = 1170
    Top = 608
    Width = 115
    Height = 30
    BevelEdges = [beBottom]
    BevelKind = bkFlat
    BevelOuter = bvNone
    Caption = 'T'#1080#1084#1087'. ='
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clLime
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 19
    SkinData.CustomColor = True
    SkinData.CustomFont = True
    SkinData.SkinSection = 'CHECKBOX'
  end
  object sPanel13: TsPanel
    Left = 1170
    Top = 578
    Width = 115
    Height = 30
    BevelEdges = [beBottom]
    BevelKind = bkFlat
    BevelOuter = bvNone
    Caption = 'U'#1080#1084#1087'. ='
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clLime
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 20
    SkinData.CustomColor = True
    SkinData.CustomFont = True
    SkinData.SkinSection = 'CHECKBOX'
  end
  object sPanel14: TsPanel
    Left = 939
    Top = 625
    Width = 145
    Height = 55
    Align = alCustom
    BevelOuter = bvLowered
    Caption = #1050#1086#1085#1090#1088#1086#1083#1100#1085#1099#1081' '#1088#1072#1079#1098#1105#1084
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 21
    SkinData.SkinSection = 'PANEL_LOW'
  end
  object sPanel15: TsPanel
    Left = 939
    Top = 678
    Width = 145
    Height = 55
    Align = alCustom
    BevelOuter = bvLowered
    Caption = #1082#1086#1085#1090#1072#1082#1090' "1"'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 22
    SkinData.SkinSection = 'PANEL_LOW'
  end
  object sPanel16: TsPanel
    Left = 939
    Top = 731
    Width = 145
    Height = 55
    Align = alCustom
    BevelOuter = bvLowered
    Caption = #1050'4 - '#1073#1083#1086#1082#1072' '#1048#1050
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 23
    SkinData.SkinSection = 'PANEL_LOW'
  end
  object sPanel17: TsPanel
    Left = 939
    Top = 784
    Width = 145
    Height = 55
    Align = alCustom
    BevelOuter = bvLowered
    Caption = #1050'5 - '#1073#1083#1086#1082#1072' '#1048#1050
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 24
    SkinData.SkinSection = 'PANEL_LOW'
  end
  object sPanel18: TsPanel
    Left = 939
    Top = 837
    Width = 145
    Height = 55
    Align = alCustom
    BevelOuter = bvLowered
    Caption = #1042#1093' 1 - '#1057#1050
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 25
    SkinData.SkinSection = 'PANEL_LOW'
  end
  object sPanel19: TsPanel
    Left = 939
    Top = 890
    Width = 145
    Height = 55
    Align = alCustom
    BevelOuter = bvLowered
    Caption = #1042#1093' 2 - '#1052#1050
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 26
    SkinData.SkinSection = 'PANEL_LOW'
  end
  object sPanel20: TsPanel
    Left = 939
    Top = 943
    Width = 145
    Height = 55
    Align = alCustom
    BevelOuter = bvLowered
    Caption = #1042#1093' 3 - '#1052#1050
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 27
    SkinData.SkinSection = 'PANEL_LOW'
  end
end
