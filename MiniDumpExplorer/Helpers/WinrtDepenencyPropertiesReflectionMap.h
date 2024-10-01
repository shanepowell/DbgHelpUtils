enum class ObjectType
{
  Library,
  Application
};

struct ObjectData
{
  ObjectType type;
  std::map<std::wstring, std::function<winrt::Microsoft::UI::Xaml::DependencyProperty()>> properties;
};

const std::map<winrt::guid, ObjectData> InterfaceIdToDependencyProperties = []()
{
  std::map<winrt::guid, ObjectData> result;
  // Microsoft.UI.Xaml.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::AdaptiveTrigger::MinWindowWidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::AdaptiveTrigger>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"minwindowwidth"s, []() { return winrt::Microsoft::UI::Xaml::AdaptiveTrigger::MinWindowWidthProperty(); } },
          { L"minwindowheight"s, []() { return winrt::Microsoft::UI::Xaml::AdaptiveTrigger::MinWindowHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::DataTemplate::ExtensionInstanceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::DataTemplate>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"extensioninstance"s, []() { return winrt::Microsoft::UI::Xaml::DataTemplate::ExtensionInstanceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::FrameworkElement::TagProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::FrameworkElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"tag"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::TagProperty(); } },
          { L"language"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::LanguageProperty(); } },
          { L"actualwidth"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::ActualWidthProperty(); } },
          { L"actualheight"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::ActualHeightProperty(); } },
          { L"width"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::WidthProperty(); } },
          { L"height"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::HeightProperty(); } },
          { L"minwidth"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::MinWidthProperty(); } },
          { L"maxwidth"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::MaxWidthProperty(); } },
          { L"minheight"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::MinHeightProperty(); } },
          { L"maxheight"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::MaxHeightProperty(); } },
          { L"horizontalalignment"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::HorizontalAlignmentProperty(); } },
          { L"verticalalignment"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::VerticalAlignmentProperty(); } },
          { L"margin"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::MarginProperty(); } },
          { L"name"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::NameProperty(); } },
          { L"datacontext"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::DataContextProperty(); } },
          { L"allowfocusoninteraction"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::AllowFocusOnInteractionProperty(); } },
          { L"focusvisualmargin"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::FocusVisualMarginProperty(); } },
          { L"focusvisualsecondarythickness"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::FocusVisualSecondaryThicknessProperty(); } },
          { L"focusvisualprimarythickness"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::FocusVisualPrimaryThicknessProperty(); } },
          { L"focusvisualsecondarybrush"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::FocusVisualSecondaryBrushProperty(); } },
          { L"focusvisualprimarybrush"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::FocusVisualPrimaryBrushProperty(); } },
          { L"allowfocuswhendisabled"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::AllowFocusWhenDisabledProperty(); } },
          { L"style"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::StyleProperty(); } },
          { L"flowdirection"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::FlowDirectionProperty(); } },
          { L"requestedtheme"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::RequestedThemeProperty(); } },
          { L"actualtheme"s, []() { return winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::StateTrigger::IsActiveProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::StateTrigger>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isactive"s, []() { return winrt::Microsoft::UI::Xaml::StateTrigger::IsActiveProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::UIElement::AllowDropProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::UIElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"allowdrop"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::AllowDropProperty(); } },
          { L"opacity"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::OpacityProperty(); } },
          { L"clip"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::ClipProperty(); } },
          { L"rendertransform"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::RenderTransformProperty(); } },
          { L"projection"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::ProjectionProperty(); } },
          { L"transform3d"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::Transform3DProperty(); } },
          { L"rendertransformorigin"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::RenderTransformOriginProperty(); } },
          { L"ishittestvisible"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::IsHitTestVisibleProperty(); } },
          { L"visibility"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::VisibilityProperty(); } },
          { L"uselayoutrounding"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::UseLayoutRoundingProperty(); } },
          { L"transitions"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::TransitionsProperty(); } },
          { L"cachemode"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::CacheModeProperty(); } },
          { L"istapenabled"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::IsTapEnabledProperty(); } },
          { L"isdoubletapenabled"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::IsDoubleTapEnabledProperty(); } },
          { L"candrag"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::CanDragProperty(); } },
          { L"isrighttapenabled"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::IsRightTapEnabledProperty(); } },
          { L"isholdingenabled"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::IsHoldingEnabledProperty(); } },
          { L"manipulationmode"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::ManipulationModeProperty(); } },
          { L"pointercaptures"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::PointerCapturesProperty(); } },
          { L"contextflyout"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::ContextFlyoutProperty(); } },
          { L"compositemode"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::CompositeModeProperty(); } },
          { L"lights"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::LightsProperty(); } },
          { L"canbescrollanchor"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::CanBeScrollAnchorProperty(); } },
          { L"exitdisplaymodeonaccesskeyinvoked"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::ExitDisplayModeOnAccessKeyInvokedProperty(); } },
          { L"isaccesskeyscope"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::IsAccessKeyScopeProperty(); } },
          { L"accesskeyscopeowner"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::AccessKeyScopeOwnerProperty(); } },
          { L"accesskey"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::AccessKeyProperty(); } },
          { L"keytipplacementmode"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::KeyTipPlacementModeProperty(); } },
          { L"keytiphorizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::KeyTipHorizontalOffsetProperty(); } },
          { L"keytipverticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::KeyTipVerticalOffsetProperty(); } },
          { L"keytiptarget"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::KeyTipTargetProperty(); } },
          { L"xyfocuskeyboardnavigation"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::XYFocusKeyboardNavigationProperty(); } },
          { L"xyfocusupnavigationstrategy"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::XYFocusUpNavigationStrategyProperty(); } },
          { L"xyfocusdownnavigationstrategy"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::XYFocusDownNavigationStrategyProperty(); } },
          { L"xyfocusleftnavigationstrategy"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::XYFocusLeftNavigationStrategyProperty(); } },
          { L"xyfocusrightnavigationstrategy"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::XYFocusRightNavigationStrategyProperty(); } },
          { L"keyboardacceleratorplacementtarget"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::KeyboardAcceleratorPlacementTargetProperty(); } },
          { L"keyboardacceleratorplacementmode"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::KeyboardAcceleratorPlacementModeProperty(); } },
          { L"highcontrastadjustment"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::HighContrastAdjustmentProperty(); } },
          { L"tabfocusnavigation"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::TabFocusNavigationProperty(); } },
          { L"shadow"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::ShadowProperty(); } },
          { L"focusstate"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::FocusStateProperty(); } },
          { L"usesystemfocusvisuals"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::UseSystemFocusVisualsProperty(); } },
          { L"xyfocusleft"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::XYFocusLeftProperty(); } },
          { L"xyfocusright"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::XYFocusRightProperty(); } },
          { L"xyfocusup"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::XYFocusUpProperty(); } },
          { L"xyfocusdown"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::XYFocusDownProperty(); } },
          { L"istabstop"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::IsTabStopProperty(); } },
          { L"tabindex"s, []() { return winrt::Microsoft::UI::Xaml::UIElement::TabIndexProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::VisualStateManager::CustomVisualStateManagerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::VisualStateManager>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"customvisualstatemanager"s, []() { return winrt::Microsoft::UI::Xaml::VisualStateManager::CustomVisualStateManagerProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Automation.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::AnnotationPatternIdentifiers::AnnotationTypeIdProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::AnnotationPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"annotationtypeid"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AnnotationPatternIdentifiers::AnnotationTypeIdProperty(); } },
          { L"annotationtypename"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AnnotationPatternIdentifiers::AnnotationTypeNameProperty(); } },
          { L"author"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AnnotationPatternIdentifiers::AuthorProperty(); } },
          { L"datetime"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AnnotationPatternIdentifiers::DateTimeProperty(); } },
          { L"target"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AnnotationPatternIdentifiers::TargetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::AutomationAnnotation::TypeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::AutomationAnnotation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"type"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationAnnotation::TypeProperty(); } },
          { L"element"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationAnnotation::ElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::AcceleratorKeyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"acceleratorkey"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::AcceleratorKeyProperty(); } },
          { L"accesskey"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::AccessKeyProperty(); } },
          { L"automationid"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::AutomationIdProperty(); } },
          { L"boundingrectangle"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::BoundingRectangleProperty(); } },
          { L"classname"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::ClassNameProperty(); } },
          { L"clickablepoint"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::ClickablePointProperty(); } },
          { L"controltype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::ControlTypeProperty(); } },
          { L"haskeyboardfocus"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::HasKeyboardFocusProperty(); } },
          { L"helptext"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::HelpTextProperty(); } },
          { L"iscontentelement"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsContentElementProperty(); } },
          { L"iscontrolelement"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsControlElementProperty(); } },
          { L"isenabled"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsEnabledProperty(); } },
          { L"iskeyboardfocusable"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsKeyboardFocusableProperty(); } },
          { L"isoffscreen"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsOffscreenProperty(); } },
          { L"ispassword"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsPasswordProperty(); } },
          { L"isrequiredforform"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsRequiredForFormProperty(); } },
          { L"itemstatus"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::ItemStatusProperty(); } },
          { L"itemtype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::ItemTypeProperty(); } },
          { L"labeledby"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::LabeledByProperty(); } },
          { L"localizedcontroltype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::LocalizedControlTypeProperty(); } },
          { L"name"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::NameProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::OrientationProperty(); } },
          { L"livesetting"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::LiveSettingProperty(); } },
          { L"controlledpeers"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::ControlledPeersProperty(); } },
          { L"positioninset"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::PositionInSetProperty(); } },
          { L"sizeofset"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::SizeOfSetProperty(); } },
          { L"level"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::LevelProperty(); } },
          { L"annotations"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::AnnotationsProperty(); } },
          { L"landmarktype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::LandmarkTypeProperty(); } },
          { L"localizedlandmarktype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::LocalizedLandmarkTypeProperty(); } },
          { L"isperipheral"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsPeripheralProperty(); } },
          { L"isdatavalidforform"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsDataValidForFormProperty(); } },
          { L"fulldescription"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::FullDescriptionProperty(); } },
          { L"describedby"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::DescribedByProperty(); } },
          { L"flowsto"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::FlowsToProperty(); } },
          { L"flowsfrom"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::FlowsFromProperty(); } },
          { L"culture"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::CultureProperty(); } },
          { L"headinglevel"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::HeadingLevelProperty(); } },
          { L"isdialog"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationElementIdentifiers::IsDialogProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::AcceleratorKeyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::AutomationProperties>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"acceleratorkey"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::AcceleratorKeyProperty(); } },
          { L"accesskey"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::AccessKeyProperty(); } },
          { L"automationid"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::AutomationIdProperty(); } },
          { L"helptext"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::HelpTextProperty(); } },
          { L"isrequiredforform"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::IsRequiredForFormProperty(); } },
          { L"itemstatus"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::ItemStatusProperty(); } },
          { L"itemtype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::ItemTypeProperty(); } },
          { L"labeledby"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::LabeledByProperty(); } },
          { L"name"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::NameProperty(); } },
          { L"livesetting"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::LiveSettingProperty(); } },
          { L"accessibilityview"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::AccessibilityViewProperty(); } },
          { L"controlledpeers"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::ControlledPeersProperty(); } },
          { L"positioninset"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::PositionInSetProperty(); } },
          { L"sizeofset"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::SizeOfSetProperty(); } },
          { L"level"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::LevelProperty(); } },
          { L"annotations"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::AnnotationsProperty(); } },
          { L"landmarktype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::LandmarkTypeProperty(); } },
          { L"localizedlandmarktype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::LocalizedLandmarkTypeProperty(); } },
          { L"isperipheral"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::IsPeripheralProperty(); } },
          { L"isdatavalidforform"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::IsDataValidForFormProperty(); } },
          { L"fulldescription"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::FullDescriptionProperty(); } },
          { L"localizedcontroltype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::LocalizedControlTypeProperty(); } },
          { L"describedby"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::DescribedByProperty(); } },
          { L"flowsto"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::FlowsToProperty(); } },
          { L"flowsfrom"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::FlowsFromProperty(); } },
          { L"culture"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::CultureProperty(); } },
          { L"headinglevel"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::HeadingLevelProperty(); } },
          { L"isdialog"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::IsDialogProperty(); } },
          { L"automationcontroltype"s, []() { return winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::AutomationControlTypeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::DockPatternIdentifiers::DockPositionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::DockPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"dockposition"s, []() { return winrt::Microsoft::UI::Xaml::Automation::DockPatternIdentifiers::DockPositionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::DragPatternIdentifiers::DropEffectProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::DragPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"dropeffect"s, []() { return winrt::Microsoft::UI::Xaml::Automation::DragPatternIdentifiers::DropEffectProperty(); } },
          { L"dropeffects"s, []() { return winrt::Microsoft::UI::Xaml::Automation::DragPatternIdentifiers::DropEffectsProperty(); } },
          { L"grabbeditems"s, []() { return winrt::Microsoft::UI::Xaml::Automation::DragPatternIdentifiers::GrabbedItemsProperty(); } },
          { L"isgrabbed"s, []() { return winrt::Microsoft::UI::Xaml::Automation::DragPatternIdentifiers::IsGrabbedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::DropTargetPatternIdentifiers::DropTargetEffectProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::DropTargetPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"droptargeteffect"s, []() { return winrt::Microsoft::UI::Xaml::Automation::DropTargetPatternIdentifiers::DropTargetEffectProperty(); } },
          { L"droptargeteffects"s, []() { return winrt::Microsoft::UI::Xaml::Automation::DropTargetPatternIdentifiers::DropTargetEffectsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::ExpandCollapsePatternIdentifiers::ExpandCollapseStateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::ExpandCollapsePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"expandcollapsestate"s, []() { return winrt::Microsoft::UI::Xaml::Automation::ExpandCollapsePatternIdentifiers::ExpandCollapseStateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::GridItemPatternIdentifiers::ColumnProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::GridItemPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"column"s, []() { return winrt::Microsoft::UI::Xaml::Automation::GridItemPatternIdentifiers::ColumnProperty(); } },
          { L"columnspan"s, []() { return winrt::Microsoft::UI::Xaml::Automation::GridItemPatternIdentifiers::ColumnSpanProperty(); } },
          { L"containinggrid"s, []() { return winrt::Microsoft::UI::Xaml::Automation::GridItemPatternIdentifiers::ContainingGridProperty(); } },
          { L"row"s, []() { return winrt::Microsoft::UI::Xaml::Automation::GridItemPatternIdentifiers::RowProperty(); } },
          { L"rowspan"s, []() { return winrt::Microsoft::UI::Xaml::Automation::GridItemPatternIdentifiers::RowSpanProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::GridPatternIdentifiers::ColumnCountProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::GridPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"columncount"s, []() { return winrt::Microsoft::UI::Xaml::Automation::GridPatternIdentifiers::ColumnCountProperty(); } },
          { L"rowcount"s, []() { return winrt::Microsoft::UI::Xaml::Automation::GridPatternIdentifiers::RowCountProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::MultipleViewPatternIdentifiers::CurrentViewProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::MultipleViewPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"currentview"s, []() { return winrt::Microsoft::UI::Xaml::Automation::MultipleViewPatternIdentifiers::CurrentViewProperty(); } },
          { L"supportedviews"s, []() { return winrt::Microsoft::UI::Xaml::Automation::MultipleViewPatternIdentifiers::SupportedViewsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::RangeValuePatternIdentifiers::IsReadOnlyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::RangeValuePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isreadonly"s, []() { return winrt::Microsoft::UI::Xaml::Automation::RangeValuePatternIdentifiers::IsReadOnlyProperty(); } },
          { L"largechange"s, []() { return winrt::Microsoft::UI::Xaml::Automation::RangeValuePatternIdentifiers::LargeChangeProperty(); } },
          { L"maximum"s, []() { return winrt::Microsoft::UI::Xaml::Automation::RangeValuePatternIdentifiers::MaximumProperty(); } },
          { L"minimum"s, []() { return winrt::Microsoft::UI::Xaml::Automation::RangeValuePatternIdentifiers::MinimumProperty(); } },
          { L"smallchange"s, []() { return winrt::Microsoft::UI::Xaml::Automation::RangeValuePatternIdentifiers::SmallChangeProperty(); } },
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Automation::RangeValuePatternIdentifiers::ValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::ScrollPatternIdentifiers::HorizontallyScrollableProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::ScrollPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"horizontallyscrollable"s, []() { return winrt::Microsoft::UI::Xaml::Automation::ScrollPatternIdentifiers::HorizontallyScrollableProperty(); } },
          { L"horizontalscrollpercent"s, []() { return winrt::Microsoft::UI::Xaml::Automation::ScrollPatternIdentifiers::HorizontalScrollPercentProperty(); } },
          { L"horizontalviewsize"s, []() { return winrt::Microsoft::UI::Xaml::Automation::ScrollPatternIdentifiers::HorizontalViewSizeProperty(); } },
          { L"verticallyscrollable"s, []() { return winrt::Microsoft::UI::Xaml::Automation::ScrollPatternIdentifiers::VerticallyScrollableProperty(); } },
          { L"verticalscrollpercent"s, []() { return winrt::Microsoft::UI::Xaml::Automation::ScrollPatternIdentifiers::VerticalScrollPercentProperty(); } },
          { L"verticalviewsize"s, []() { return winrt::Microsoft::UI::Xaml::Automation::ScrollPatternIdentifiers::VerticalViewSizeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::SelectionItemPatternIdentifiers::IsSelectedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::SelectionItemPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isselected"s, []() { return winrt::Microsoft::UI::Xaml::Automation::SelectionItemPatternIdentifiers::IsSelectedProperty(); } },
          { L"selectioncontainer"s, []() { return winrt::Microsoft::UI::Xaml::Automation::SelectionItemPatternIdentifiers::SelectionContainerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::SelectionPatternIdentifiers::CanSelectMultipleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::SelectionPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"canselectmultiple"s, []() { return winrt::Microsoft::UI::Xaml::Automation::SelectionPatternIdentifiers::CanSelectMultipleProperty(); } },
          { L"isselectionrequired"s, []() { return winrt::Microsoft::UI::Xaml::Automation::SelectionPatternIdentifiers::IsSelectionRequiredProperty(); } },
          { L"selection"s, []() { return winrt::Microsoft::UI::Xaml::Automation::SelectionPatternIdentifiers::SelectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::SpreadsheetItemPatternIdentifiers::FormulaProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::SpreadsheetItemPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"formula"s, []() { return winrt::Microsoft::UI::Xaml::Automation::SpreadsheetItemPatternIdentifiers::FormulaProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::StylesPatternIdentifiers::ExtendedPropertiesProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::StylesPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"extendedproperties"s, []() { return winrt::Microsoft::UI::Xaml::Automation::StylesPatternIdentifiers::ExtendedPropertiesProperty(); } },
          { L"fillcolor"s, []() { return winrt::Microsoft::UI::Xaml::Automation::StylesPatternIdentifiers::FillColorProperty(); } },
          { L"fillpatterncolor"s, []() { return winrt::Microsoft::UI::Xaml::Automation::StylesPatternIdentifiers::FillPatternColorProperty(); } },
          { L"fillpatternstyle"s, []() { return winrt::Microsoft::UI::Xaml::Automation::StylesPatternIdentifiers::FillPatternStyleProperty(); } },
          { L"shape"s, []() { return winrt::Microsoft::UI::Xaml::Automation::StylesPatternIdentifiers::ShapeProperty(); } },
          { L"styleid"s, []() { return winrt::Microsoft::UI::Xaml::Automation::StylesPatternIdentifiers::StyleIdProperty(); } },
          { L"stylename"s, []() { return winrt::Microsoft::UI::Xaml::Automation::StylesPatternIdentifiers::StyleNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::TableItemPatternIdentifiers::ColumnHeaderItemsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::TableItemPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"columnheaderitems"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TableItemPatternIdentifiers::ColumnHeaderItemsProperty(); } },
          { L"rowheaderitems"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TableItemPatternIdentifiers::RowHeaderItemsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::TablePatternIdentifiers::ColumnHeadersProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::TablePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"columnheaders"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TablePatternIdentifiers::ColumnHeadersProperty(); } },
          { L"rowheaders"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TablePatternIdentifiers::RowHeadersProperty(); } },
          { L"roworcolumnmajor"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TablePatternIdentifiers::RowOrColumnMajorProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::TogglePatternIdentifiers::ToggleStateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::TogglePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"togglestate"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TogglePatternIdentifiers::ToggleStateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::TransformPattern2Identifiers::CanZoomProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::TransformPattern2Identifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"canzoom"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TransformPattern2Identifiers::CanZoomProperty(); } },
          { L"zoomlevel"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TransformPattern2Identifiers::ZoomLevelProperty(); } },
          { L"maxzoom"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TransformPattern2Identifiers::MaxZoomProperty(); } },
          { L"minzoom"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TransformPattern2Identifiers::MinZoomProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::TransformPatternIdentifiers::CanMoveProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::TransformPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"canmove"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TransformPatternIdentifiers::CanMoveProperty(); } },
          { L"canresize"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TransformPatternIdentifiers::CanResizeProperty(); } },
          { L"canrotate"s, []() { return winrt::Microsoft::UI::Xaml::Automation::TransformPatternIdentifiers::CanRotateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::ValuePatternIdentifiers::IsReadOnlyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::ValuePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isreadonly"s, []() { return winrt::Microsoft::UI::Xaml::Automation::ValuePatternIdentifiers::IsReadOnlyProperty(); } },
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Automation::ValuePatternIdentifiers::ValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::WindowPatternIdentifiers::CanMaximizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::WindowPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"canmaximize"s, []() { return winrt::Microsoft::UI::Xaml::Automation::WindowPatternIdentifiers::CanMaximizeProperty(); } },
          { L"canminimize"s, []() { return winrt::Microsoft::UI::Xaml::Automation::WindowPatternIdentifiers::CanMinimizeProperty(); } },
          { L"ismodal"s, []() { return winrt::Microsoft::UI::Xaml::Automation::WindowPatternIdentifiers::IsModalProperty(); } },
          { L"istopmost"s, []() { return winrt::Microsoft::UI::Xaml::Automation::WindowPatternIdentifiers::IsTopmostProperty(); } },
          { L"windowinteractionstate"s, []() { return winrt::Microsoft::UI::Xaml::Automation::WindowPatternIdentifiers::WindowInteractionStateProperty(); } },
          { L"windowvisualstate"s, []() { return winrt::Microsoft::UI::Xaml::Automation::WindowPatternIdentifiers::WindowVisualStateProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Automation.Peers.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Automation::Peers::AutomationPeerAnnotation::TypeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Automation::Peers::AutomationPeerAnnotation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"type"s, []() { return winrt::Microsoft::UI::Xaml::Automation::Peers::AutomationPeerAnnotation::TypeProperty(); } },
          { L"peer"s, []() { return winrt::Microsoft::UI::Xaml::Automation::Peers::AutomationPeerAnnotation::PeerProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Controls.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AnimatedIcon::StateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AnimatedIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"state"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedIcon::StateProperty(); } },
          { L"source"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedIcon::SourceProperty(); } },
          { L"fallbackiconsource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedIcon::FallbackIconSourceProperty(); } },
          { L"mirroredwhenrighttoleft"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedIcon::MirroredWhenRightToLeftProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AnimatedIconSource::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AnimatedIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedIconSource::SourceProperty(); } },
          { L"fallbackiconsource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedIconSource::FallbackIconSourceProperty(); } },
          { L"mirroredwhenrighttoleft"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedIconSource::MirroredWhenRightToLeftProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::AutoPlayProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"autoplay"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::AutoPlayProperty(); } },
          { L"diagnostics"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::DiagnosticsProperty(); } },
          { L"duration"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::DurationProperty(); } },
          { L"fallbackcontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::FallbackContentProperty(); } },
          { L"isanimatedvisualloaded"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::IsAnimatedVisualLoadedProperty(); } },
          { L"isplaying"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::IsPlayingProperty(); } },
          { L"playbackrate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::PlaybackRateProperty(); } },
          { L"source"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::SourceProperty(); } },
          { L"stretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::StretchProperty(); } },
          { L"animationoptimization"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer::AnimationOptimizationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AnnotatedScrollBar::LabelsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AnnotatedScrollBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"labels"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnnotatedScrollBar::LabelsProperty(); } },
          { L"labeltemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnnotatedScrollBar::LabelTemplateProperty(); } },
          { L"detaillabeltemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnnotatedScrollBar::DetailLabelTemplateProperty(); } },
          { L"smallchange"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AnnotatedScrollBar::SmallChangeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AppBar::IsOpenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AppBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBar::IsOpenProperty(); } },
          { L"issticky"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBar::IsStickyProperty(); } },
          { L"closeddisplaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBar::ClosedDisplayModeProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBar::LightDismissOverlayModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AppBarButton::LabelProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AppBarButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"label"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarButton::LabelProperty(); } },
          { L"icon"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarButton::IconProperty(); } },
          { L"labelposition"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarButton::LabelPositionProperty(); } },
          { L"keyboardacceleratortextoverride"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarButton::KeyboardAcceleratorTextOverrideProperty(); } },
          { L"iscompact"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarButton::IsCompactProperty(); } },
          { L"isinoverflow"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarButton::IsInOverflowProperty(); } },
          { L"dynamicoverfloworder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarButton::DynamicOverflowOrderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AppBarElementContainer::IsCompactProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AppBarElementContainer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iscompact"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarElementContainer::IsCompactProperty(); } },
          { L"isinoverflow"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarElementContainer::IsInOverflowProperty(); } },
          { L"dynamicoverfloworder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarElementContainer::DynamicOverflowOrderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AppBarSeparator::IsCompactProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AppBarSeparator>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iscompact"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarSeparator::IsCompactProperty(); } },
          { L"isinoverflow"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarSeparator::IsInOverflowProperty(); } },
          { L"dynamicoverfloworder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarSeparator::DynamicOverflowOrderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AppBarToggleButton::LabelProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AppBarToggleButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"label"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarToggleButton::LabelProperty(); } },
          { L"icon"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarToggleButton::IconProperty(); } },
          { L"labelposition"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarToggleButton::LabelPositionProperty(); } },
          { L"keyboardacceleratortextoverride"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarToggleButton::KeyboardAcceleratorTextOverrideProperty(); } },
          { L"iscompact"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarToggleButton::IsCompactProperty(); } },
          { L"isinoverflow"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarToggleButton::IsInOverflowProperty(); } },
          { L"dynamicoverfloworder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AppBarToggleButton::DynamicOverflowOrderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::MaxSuggestionListHeightProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"maxsuggestionlistheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::MaxSuggestionListHeightProperty(); } },
          { L"issuggestionlistopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::IsSuggestionListOpenProperty(); } },
          { L"textmemberpath"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::TextMemberPathProperty(); } },
          { L"text"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::TextProperty(); } },
          { L"updatetextonselect"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::UpdateTextOnSelectProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::PlaceholderTextProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::HeaderProperty(); } },
          { L"automaximizesuggestionarea"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::AutoMaximizeSuggestionAreaProperty(); } },
          { L"textboxstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::TextBoxStyleProperty(); } },
          { L"queryicon"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::QueryIconProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::LightDismissOverlayModeProperty(); } },
          { L"description"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox::DescriptionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs::ReasonProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"reason"s, []() { return winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs::ReasonProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::BitmapIcon::UriSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::BitmapIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"urisource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::BitmapIcon::UriSourceProperty(); } },
          { L"showasmonochrome"s, []() { return winrt::Microsoft::UI::Xaml::Controls::BitmapIcon::ShowAsMonochromeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::BitmapIconSource::UriSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::BitmapIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"urisource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::BitmapIconSource::UriSourceProperty(); } },
          { L"showasmonochrome"s, []() { return winrt::Microsoft::UI::Xaml::Controls::BitmapIconSource::ShowAsMonochromeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Border::BorderBrushProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Border>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"borderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Border::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Border::BorderThicknessProperty(); } },
          { L"background"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Border::BackgroundProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Border::BackgroundSizingProperty(); } },
          { L"cornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Border::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Border::PaddingProperty(); } },
          { L"childtransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Border::ChildTransitionsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::BreadcrumbBar::ItemsSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::BreadcrumbBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::BreadcrumbBar::ItemsSourceProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::BreadcrumbBar::ItemTemplateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Button::FlyoutProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Button>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"flyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Button::FlyoutProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::DateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"date"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::DateProperty(); } },
          { L"iscalendaropen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::IsCalendarOpenProperty(); } },
          { L"dateformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::DateFormatProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::PlaceholderTextProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::HeaderTemplateProperty(); } },
          { L"calendarviewstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::CalendarViewStyleProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::LightDismissOverlayModeProperty(); } },
          { L"description"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::DescriptionProperty(); } },
          { L"mindate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::MinDateProperty(); } },
          { L"maxdate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::MaxDateProperty(); } },
          { L"istodayhighlighted"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::IsTodayHighlightedProperty(); } },
          { L"displaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::DisplayModeProperty(); } },
          { L"firstdayofweek"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::FirstDayOfWeekProperty(); } },
          { L"dayofweekformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::DayOfWeekFormatProperty(); } },
          { L"calendaridentifier"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::CalendarIdentifierProperty(); } },
          { L"isoutofscopeenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::IsOutOfScopeEnabledProperty(); } },
          { L"isgrouplabelvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker::IsGroupLabelVisibleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarIdentifierProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::CalendarView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"calendaridentifier"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarIdentifierProperty(); } },
          { L"dayofweekformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::DayOfWeekFormatProperty(); } },
          { L"isgrouplabelvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::IsGroupLabelVisibleProperty(); } },
          { L"displaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::DisplayModeProperty(); } },
          { L"firstdayofweek"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstDayOfWeekProperty(); } },
          { L"isoutofscopeenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::IsOutOfScopeEnabledProperty(); } },
          { L"istodayhighlighted"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::IsTodayHighlightedProperty(); } },
          { L"maxdate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::MaxDateProperty(); } },
          { L"mindate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::MinDateProperty(); } },
          { L"numberofweeksinview"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::NumberOfWeeksInViewProperty(); } },
          { L"selecteddates"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectedDatesProperty(); } },
          { L"selectionmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectionModeProperty(); } },
          { L"templatesettings"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TemplateSettingsProperty(); } },
          { L"focusborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FocusBorderBrushProperty(); } },
          { L"selectedhoverborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectedHoverBorderBrushProperty(); } },
          { L"selectedpressedborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectedPressedBorderBrushProperty(); } },
          { L"selecteddisabledborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectedDisabledBorderBrushProperty(); } },
          { L"selectedborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectedBorderBrushProperty(); } },
          { L"hoverborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::HoverBorderBrushProperty(); } },
          { L"pressedborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::PressedBorderBrushProperty(); } },
          { L"todayselectedinnerborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TodaySelectedInnerBorderBrushProperty(); } },
          { L"blackoutstrikethroughbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::BlackoutStrikethroughBrushProperty(); } },
          { L"calendaritemborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarItemBorderBrushProperty(); } },
          { L"blackoutbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::BlackoutBackgroundProperty(); } },
          { L"outofscopebackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::OutOfScopeBackgroundProperty(); } },
          { L"calendaritembackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarItemBackgroundProperty(); } },
          { L"calendaritemhoverbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarItemHoverBackgroundProperty(); } },
          { L"calendaritempressedbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarItemPressedBackgroundProperty(); } },
          { L"calendaritemdisabledbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarItemDisabledBackgroundProperty(); } },
          { L"todaybackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TodayBackgroundProperty(); } },
          { L"todayblackoutbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TodayBlackoutBackgroundProperty(); } },
          { L"todayhoverbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TodayHoverBackgroundProperty(); } },
          { L"todaypressedbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TodayPressedBackgroundProperty(); } },
          { L"todaydisabledbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TodayDisabledBackgroundProperty(); } },
          { L"pressedforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::PressedForegroundProperty(); } },
          { L"todayforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TodayForegroundProperty(); } },
          { L"blackoutforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::BlackoutForegroundProperty(); } },
          { L"todayblackoutforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TodayBlackoutForegroundProperty(); } },
          { L"selectedforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectedForegroundProperty(); } },
          { L"selectedhoverforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectedHoverForegroundProperty(); } },
          { L"selectedpressedforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectedPressedForegroundProperty(); } },
          { L"selecteddisabledforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::SelectedDisabledForegroundProperty(); } },
          { L"outofscopeforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::OutOfScopeForegroundProperty(); } },
          { L"outofscopehoverforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::OutOfScopeHoverForegroundProperty(); } },
          { L"outofscopepressedforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::OutOfScopePressedForegroundProperty(); } },
          { L"calendaritemforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarItemForegroundProperty(); } },
          { L"disabledforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::DisabledForegroundProperty(); } },
          { L"dayitemfontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::DayItemFontFamilyProperty(); } },
          { L"dayitemfontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::DayItemFontSizeProperty(); } },
          { L"dayitemfontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::DayItemFontStyleProperty(); } },
          { L"dayitemfontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::DayItemFontWeightProperty(); } },
          { L"todayfontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::TodayFontWeightProperty(); } },
          { L"firstofmonthlabelfontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelFontFamilyProperty(); } },
          { L"firstofmonthlabelfontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelFontSizeProperty(); } },
          { L"firstofmonthlabelfontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelFontStyleProperty(); } },
          { L"firstofmonthlabelfontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelFontWeightProperty(); } },
          { L"monthyearitemfontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::MonthYearItemFontFamilyProperty(); } },
          { L"monthyearitemfontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::MonthYearItemFontSizeProperty(); } },
          { L"monthyearitemfontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::MonthYearItemFontStyleProperty(); } },
          { L"monthyearitemfontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::MonthYearItemFontWeightProperty(); } },
          { L"firstofyeardecadelabelfontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelFontFamilyProperty(); } },
          { L"firstofyeardecadelabelfontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelFontSizeProperty(); } },
          { L"firstofyeardecadelabelfontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelFontStyleProperty(); } },
          { L"firstofyeardecadelabelfontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelFontWeightProperty(); } },
          { L"dayitemmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::DayItemMarginProperty(); } },
          { L"monthyearitemmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::MonthYearItemMarginProperty(); } },
          { L"firstofmonthlabelmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelMarginProperty(); } },
          { L"firstofyeardecadelabelmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelMarginProperty(); } },
          { L"horizontaldayitemalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::HorizontalDayItemAlignmentProperty(); } },
          { L"verticaldayitemalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::VerticalDayItemAlignmentProperty(); } },
          { L"horizontalfirstofmonthlabelalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::HorizontalFirstOfMonthLabelAlignmentProperty(); } },
          { L"verticalfirstofmonthlabelalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::VerticalFirstOfMonthLabelAlignmentProperty(); } },
          { L"calendaritemborderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarItemBorderThicknessProperty(); } },
          { L"calendarviewdayitemstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarViewDayItemStyleProperty(); } },
          { L"calendaritemcornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarView::CalendarItemCornerRadiusProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::CalendarViewDayItem::IsBlackoutProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::CalendarViewDayItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isblackout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarViewDayItem::IsBlackoutProperty(); } },
          { L"date"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CalendarViewDayItem::DateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Canvas::LeftProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Canvas>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"left"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Canvas::LeftProperty(); } },
          { L"top"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Canvas::TopProperty(); } },
          { L"zindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Canvas::ZIndexProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ColorPicker::ColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ColorPicker>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"color"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::ColorProperty(); } },
          { L"previouscolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::PreviousColorProperty(); } },
          { L"isalphaenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::IsAlphaEnabledProperty(); } },
          { L"iscolorspectrumvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::IsColorSpectrumVisibleProperty(); } },
          { L"iscolorpreviewvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::IsColorPreviewVisibleProperty(); } },
          { L"iscolorslidervisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::IsColorSliderVisibleProperty(); } },
          { L"isalphaslidervisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::IsAlphaSliderVisibleProperty(); } },
          { L"ismorebuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::IsMoreButtonVisibleProperty(); } },
          { L"iscolorchanneltextinputvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::IsColorChannelTextInputVisibleProperty(); } },
          { L"isalphatextinputvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::IsAlphaTextInputVisibleProperty(); } },
          { L"ishexinputvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::IsHexInputVisibleProperty(); } },
          { L"minhue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::MinHueProperty(); } },
          { L"maxhue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::MaxHueProperty(); } },
          { L"minsaturation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::MinSaturationProperty(); } },
          { L"maxsaturation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::MaxSaturationProperty(); } },
          { L"minvalue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::MinValueProperty(); } },
          { L"maxvalue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::MaxValueProperty(); } },
          { L"colorspectrumshape"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::ColorSpectrumShapeProperty(); } },
          { L"colorspectrumcomponents"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::ColorSpectrumComponentsProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColorPicker::OrientationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ColumnDefinition::WidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ColumnDefinition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"width"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColumnDefinition::WidthProperty(); } },
          { L"maxwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColumnDefinition::MaxWidthProperty(); } },
          { L"minwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ColumnDefinition::MinWidthProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ComboBox::IsEditableProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ComboBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iseditable"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::IsEditableProperty(); } },
          { L"isdropdownopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::IsDropDownOpenProperty(); } },
          { L"maxdropdownheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::MaxDropDownHeightProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::HeaderTemplateProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::PlaceholderTextProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::LightDismissOverlayModeProperty(); } },
          { L"istextsearchenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::IsTextSearchEnabledProperty(); } },
          { L"selectionchangedtrigger"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::SelectionChangedTriggerProperty(); } },
          { L"placeholderforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::PlaceholderForegroundProperty(); } },
          { L"text"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::TextProperty(); } },
          { L"textboxstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::TextBoxStyleProperty(); } },
          { L"description"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ComboBox::DescriptionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::CommandBar::PrimaryCommandsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::CommandBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"primarycommands"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CommandBar::PrimaryCommandsProperty(); } },
          { L"secondarycommands"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CommandBar::SecondaryCommandsProperty(); } },
          { L"commandbaroverflowpresenterstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CommandBar::CommandBarOverflowPresenterStyleProperty(); } },
          { L"defaultlabelposition"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CommandBar::DefaultLabelPositionProperty(); } },
          { L"overflowbuttonvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CommandBar::OverflowButtonVisibilityProperty(); } },
          { L"isdynamicoverflowenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::CommandBar::IsDynamicOverflowEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ContentControl::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ContentControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentControl::ContentProperty(); } },
          { L"contenttemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentControl::ContentTemplateProperty(); } },
          { L"contenttemplateselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentControl::ContentTemplateSelectorProperty(); } },
          { L"contenttransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentControl::ContentTransitionsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ContentDialog::TitleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ContentDialog>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"title"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::TitleProperty(); } },
          { L"titletemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::TitleTemplateProperty(); } },
          { L"fullsizedesired"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::FullSizeDesiredProperty(); } },
          { L"primarybuttontext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::PrimaryButtonTextProperty(); } },
          { L"secondarybuttontext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::SecondaryButtonTextProperty(); } },
          { L"closebuttontext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::CloseButtonTextProperty(); } },
          { L"primarybuttoncommand"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::PrimaryButtonCommandProperty(); } },
          { L"secondarybuttoncommand"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::SecondaryButtonCommandProperty(); } },
          { L"closebuttoncommand"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::CloseButtonCommandProperty(); } },
          { L"primarybuttoncommandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::PrimaryButtonCommandParameterProperty(); } },
          { L"secondarybuttoncommandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::SecondaryButtonCommandParameterProperty(); } },
          { L"closebuttoncommandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::CloseButtonCommandParameterProperty(); } },
          { L"isprimarybuttonenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::IsPrimaryButtonEnabledProperty(); } },
          { L"issecondarybuttonenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::IsSecondaryButtonEnabledProperty(); } },
          { L"primarybuttonstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::PrimaryButtonStyleProperty(); } },
          { L"secondarybuttonstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::SecondaryButtonStyleProperty(); } },
          { L"closebuttonstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::CloseButtonStyleProperty(); } },
          { L"defaultbutton"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentDialog::DefaultButtonProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ContentPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::ContentProperty(); } },
          { L"contenttemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::ContentTemplateProperty(); } },
          { L"contenttemplateselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::ContentTemplateSelectorProperty(); } },
          { L"contenttransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::ContentTransitionsProperty(); } },
          { L"fontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::FontStretchProperty(); } },
          { L"characterspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::CharacterSpacingProperty(); } },
          { L"foreground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::ForegroundProperty(); } },
          { L"opticalmarginalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::OpticalMarginAlignmentProperty(); } },
          { L"textlinebounds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::TextLineBoundsProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::IsTextScaleFactorEnabledProperty(); } },
          { L"textwrapping"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::TextWrappingProperty(); } },
          { L"maxlines"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::MaxLinesProperty(); } },
          { L"linestackingstrategy"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::LineStackingStrategyProperty(); } },
          { L"lineheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::LineHeightProperty(); } },
          { L"borderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::BorderThicknessProperty(); } },
          { L"cornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::PaddingProperty(); } },
          { L"background"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::BackgroundProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::BackgroundSizingProperty(); } },
          { L"horizontalcontentalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::HorizontalContentAlignmentProperty(); } },
          { L"verticalcontentalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ContentPresenter::VerticalContentAlignmentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Control::IsFocusEngagementEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Control>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isfocusengagementenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::IsFocusEngagementEnabledProperty(); } },
          { L"isfocusengaged"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::IsFocusEngagedProperty(); } },
          { L"requirespointer"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::RequiresPointerProperty(); } },
          { L"fontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::FontStretchProperty(); } },
          { L"characterspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::CharacterSpacingProperty(); } },
          { L"foreground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::ForegroundProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::IsTextScaleFactorEnabledProperty(); } },
          { L"isenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::IsEnabledProperty(); } },
          { L"tabnavigation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::TabNavigationProperty(); } },
          { L"template"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::TemplateProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::PaddingProperty(); } },
          { L"horizontalcontentalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::HorizontalContentAlignmentProperty(); } },
          { L"verticalcontentalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::VerticalContentAlignmentProperty(); } },
          { L"background"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::BackgroundProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::BackgroundSizingProperty(); } },
          { L"borderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::BorderThicknessProperty(); } },
          { L"borderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::BorderBrushProperty(); } },
          { L"defaultstylekey"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::DefaultStyleKeyProperty(); } },
          { L"defaultstyleresourceuri"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::DefaultStyleResourceUriProperty(); } },
          { L"elementsoundmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::ElementSoundModeProperty(); } },
          { L"cornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::CornerRadiusProperty(); } },
          { L"istemplatefocustarget"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::IsTemplateFocusTargetProperty(); } },
          { L"istemplatekeytiptarget"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Control::IsTemplateKeyTipTargetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::DatePicker::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::DatePicker>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::HeaderTemplateProperty(); } },
          { L"calendaridentifier"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::CalendarIdentifierProperty(); } },
          { L"date"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::DateProperty(); } },
          { L"dayvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::DayVisibleProperty(); } },
          { L"monthvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::MonthVisibleProperty(); } },
          { L"yearvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::YearVisibleProperty(); } },
          { L"dayformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::DayFormatProperty(); } },
          { L"monthformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::MonthFormatProperty(); } },
          { L"yearformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::YearFormatProperty(); } },
          { L"minyear"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::MinYearProperty(); } },
          { L"maxyear"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::MaxYearProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::OrientationProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::LightDismissOverlayModeProperty(); } },
          { L"selecteddate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePicker::SelectedDateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::CalendarIdentifierProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"calendaridentifier"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::CalendarIdentifierProperty(); } },
          { L"date"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::DateProperty(); } },
          { L"dayvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::DayVisibleProperty(); } },
          { L"monthvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::MonthVisibleProperty(); } },
          { L"yearvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::YearVisibleProperty(); } },
          { L"minyear"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::MinYearProperty(); } },
          { L"maxyear"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::MaxYearProperty(); } },
          { L"dayformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::DayFormatProperty(); } },
          { L"monthformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::MonthFormatProperty(); } },
          { L"yearformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyout::YearFormatProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyoutItem::PrimaryTextProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyoutItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"primarytext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyoutItem::PrimaryTextProperty(); } },
          { L"secondarytext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyoutItem::SecondaryTextProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyoutPresenter::IsDefaultShadowEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyoutPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdefaultshadowenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::DatePickerFlyoutPresenter::IsDefaultShadowEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Expander::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Expander>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Expander::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Expander::HeaderTemplateProperty(); } },
          { L"headertemplateselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Expander::HeaderTemplateSelectorProperty(); } },
          { L"isexpanded"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Expander::IsExpandedProperty(); } },
          { L"expanddirection"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Expander::ExpandDirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::FlipView::UseTouchAnimationsForAllNavigationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::FlipView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"usetouchanimationsforallnavigation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FlipView::UseTouchAnimationsForAllNavigationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Flyout::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Flyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Flyout::ContentProperty(); } },
          { L"flyoutpresenterstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Flyout::FlyoutPresenterStyleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::FlyoutPresenter::IsDefaultShadowEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::FlyoutPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdefaultshadowenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FlyoutPresenter::IsDefaultShadowEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::FontIcon::GlyphProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::FontIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"glyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIcon::GlyphProperty(); } },
          { L"fontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIcon::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIcon::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIcon::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIcon::FontStyleProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIcon::IsTextScaleFactorEnabledProperty(); } },
          { L"mirroredwhenrighttoleft"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIcon::MirroredWhenRightToLeftProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::FontIconSource::GlyphProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::FontIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"glyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIconSource::GlyphProperty(); } },
          { L"fontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIconSource::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIconSource::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIconSource::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIconSource::FontStyleProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIconSource::IsTextScaleFactorEnabledProperty(); } },
          { L"mirroredwhenrighttoleft"s, []() { return winrt::Microsoft::UI::Xaml::Controls::FontIconSource::MirroredWhenRightToLeftProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Frame::CacheSizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Frame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"cachesize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Frame::CacheSizeProperty(); } },
          { L"cangoback"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Frame::CanGoBackProperty(); } },
          { L"cangoforward"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Frame::CanGoForwardProperty(); } },
          { L"currentsourcepagetype"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Frame::CurrentSourcePageTypeProperty(); } },
          { L"sourcepagetype"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Frame::SourcePageTypeProperty(); } },
          { L"backstackdepth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Frame::BackStackDepthProperty(); } },
          { L"backstack"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Frame::BackStackProperty(); } },
          { L"forwardstack"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Frame::ForwardStackProperty(); } },
          { L"isnavigationstackenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Frame::IsNavigationStackEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Grid::BackgroundSizingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Grid>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"backgroundsizing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::BackgroundSizingProperty(); } },
          { L"borderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::BorderThicknessProperty(); } },
          { L"cornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::PaddingProperty(); } },
          { L"rowspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::RowSpacingProperty(); } },
          { L"columnspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::ColumnSpacingProperty(); } },
          { L"row"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::RowProperty(); } },
          { L"column"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::ColumnProperty(); } },
          { L"rowspan"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::RowSpanProperty(); } },
          { L"columnspan"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Grid::ColumnSpanProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Hub::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Hub>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Hub::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Hub::HeaderTemplateProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Hub::OrientationProperty(); } },
          { L"defaultsectionindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Hub::DefaultSectionIndexProperty(); } },
          { L"semanticzoomowner"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Hub::SemanticZoomOwnerProperty(); } },
          { L"isactiveview"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Hub::IsActiveViewProperty(); } },
          { L"iszoomedinview"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Hub::IsZoomedInViewProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::HubSection::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::HubSection>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::HubSection::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::HubSection::HeaderTemplateProperty(); } },
          { L"contenttemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::HubSection::ContentTemplateProperty(); } },
          { L"isheaderinteractive"s, []() { return winrt::Microsoft::UI::Xaml::Controls::HubSection::IsHeaderInteractiveProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::HyperlinkButton::NavigateUriProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::HyperlinkButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"navigateuri"s, []() { return winrt::Microsoft::UI::Xaml::Controls::HyperlinkButton::NavigateUriProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::IconElement::ForegroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::IconElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"foreground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::IconElement::ForegroundProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::IconSource::ForegroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::IconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"foreground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::IconSource::ForegroundProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::IconSourceElement::IconSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::IconSourceElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iconsource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::IconSourceElement::IconSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Image::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Image>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Image::SourceProperty(); } },
          { L"stretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Image::StretchProperty(); } },
          { L"ninegrid"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Image::NineGridProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ImageIcon::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ImageIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ImageIcon::SourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ImageIconSource::ImageSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ImageIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"imagesource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ImageIconSource::ImageSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::InfoBadge::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::InfoBadge>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBadge::ValueProperty(); } },
          { L"iconsource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBadge::IconSourceProperty(); } },
          { L"templatesettings"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBadge::TemplateSettingsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::InfoBadgeTemplateSettings::InfoBadgeCornerRadiusProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::InfoBadgeTemplateSettings>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"infobadgecornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBadgeTemplateSettings::InfoBadgeCornerRadiusProperty(); } },
          { L"iconelement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBadgeTemplateSettings::IconElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::InfoBar::IsOpenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::InfoBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::IsOpenProperty(); } },
          { L"title"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::TitleProperty(); } },
          { L"message"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::MessageProperty(); } },
          { L"severity"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::SeverityProperty(); } },
          { L"iconsource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::IconSourceProperty(); } },
          { L"isiconvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::IsIconVisibleProperty(); } },
          { L"isclosable"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::IsClosableProperty(); } },
          { L"closebuttonstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::CloseButtonStyleProperty(); } },
          { L"closebuttoncommand"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::CloseButtonCommandProperty(); } },
          { L"closebuttoncommandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::CloseButtonCommandParameterProperty(); } },
          { L"actionbutton"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::ActionButtonProperty(); } },
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::ContentProperty(); } },
          { L"contenttemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::ContentTemplateProperty(); } },
          { L"templatesettings"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBar::TemplateSettingsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::InfoBarTemplateSettings::IconElementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::InfoBarTemplateSettings>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iconelement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::InfoBarTemplateSettings::IconElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ItemContainer::ChildProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ItemContainer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"child"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemContainer::ChildProperty(); } },
          { L"isselected"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemContainer::IsSelectedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ItemsControl::ItemsSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ItemsControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::ItemsSourceProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::ItemTemplateProperty(); } },
          { L"itemtemplateselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::ItemTemplateSelectorProperty(); } },
          { L"itemspanel"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::ItemsPanelProperty(); } },
          { L"displaymemberpath"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::DisplayMemberPathProperty(); } },
          { L"itemcontainerstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::ItemContainerStyleProperty(); } },
          { L"itemcontainerstyleselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::ItemContainerStyleSelectorProperty(); } },
          { L"itemcontainertransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::ItemContainerTransitionsProperty(); } },
          { L"groupstyleselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::GroupStyleSelectorProperty(); } },
          { L"isgrouping"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsControl::IsGroupingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ItemsPresenter::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ItemsPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsPresenter::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsPresenter::HeaderTemplateProperty(); } },
          { L"headertransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsPresenter::HeaderTransitionsProperty(); } },
          { L"footer"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsPresenter::FooterProperty(); } },
          { L"footertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsPresenter::FooterTemplateProperty(); } },
          { L"footertransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsPresenter::FooterTransitionsProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsPresenter::PaddingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater::ItemsSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater::ItemsSourceProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater::ItemTemplateProperty(); } },
          { L"layout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater::LayoutProperty(); } },
          { L"horizontalcachelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater::HorizontalCacheLengthProperty(); } },
          { L"verticalcachelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater::VerticalCacheLengthProperty(); } },
          { L"background"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater::BackgroundProperty(); } },
          { L"itemtransitionprovider"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater::ItemTransitionProviderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ItemsStackPanel::GroupPaddingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ItemsStackPanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"grouppadding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsStackPanel::GroupPaddingProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsStackPanel::OrientationProperty(); } },
          { L"groupheaderplacement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsStackPanel::GroupHeaderPlacementProperty(); } },
          { L"cachelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsStackPanel::CacheLengthProperty(); } },
          { L"arestickygroupheadersenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsStackPanel::AreStickyGroupHeadersEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ItemsView::IsItemInvokedEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ItemsView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isiteminvokedenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::IsItemInvokedEnabledProperty(); } },
          { L"itemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::ItemsSourceProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::ItemTemplateProperty(); } },
          { L"layout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::LayoutProperty(); } },
          { L"selectionmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::SelectionModeProperty(); } },
          { L"itemtransitionprovider"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::ItemTransitionProviderProperty(); } },
          { L"currentitemindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::CurrentItemIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::SelectedItemProperty(); } },
          { L"scrollview"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::ScrollViewProperty(); } },
          { L"verticalscrollcontroller"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsView::VerticalScrollControllerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid::GroupPaddingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"grouppadding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid::GroupPaddingProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid::OrientationProperty(); } },
          { L"maximumrowsorcolumns"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid::MaximumRowsOrColumnsProperty(); } },
          { L"itemwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid::ItemWidthProperty(); } },
          { L"itemheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid::ItemHeightProperty(); } },
          { L"groupheaderplacement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid::GroupHeaderPlacementProperty(); } },
          { L"cachelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid::CacheLengthProperty(); } },
          { L"arestickygroupheadersenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid::AreStickyGroupHeadersEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::LinedFlowLayout::ItemsJustificationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::LinedFlowLayout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemsjustification"s, []() { return winrt::Microsoft::UI::Xaml::Controls::LinedFlowLayout::ItemsJustificationProperty(); } },
          { L"itemsstretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::LinedFlowLayout::ItemsStretchProperty(); } },
          { L"minitemspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::LinedFlowLayout::MinItemSpacingProperty(); } },
          { L"linespacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::LinedFlowLayout::LineSpacingProperty(); } },
          { L"lineheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::LinedFlowLayout::LineHeightProperty(); } },
          { L"actuallineheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::LinedFlowLayout::ActualLineHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ListBox::SelectionModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ListBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectionmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListBox::SelectionModeProperty(); } },
          { L"singleselectionfollowsfocus"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListBox::SingleSelectionFollowsFocusProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout::ItemsSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout::ItemsSourceProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout::ItemTemplateProperty(); } },
          { L"displaymemberpath"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout::DisplayMemberPathProperty(); } },
          { L"selectionmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout::SelectionModeProperty(); } },
          { L"selectedindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout::SelectedItemProperty(); } },
          { L"selectedvalue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout::SelectedValueProperty(); } },
          { L"selectedvaluepath"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListPickerFlyout::SelectedValuePathProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ListViewBase::SelectionModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ListViewBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectionmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::SelectionModeProperty(); } },
          { L"isswipeenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::IsSwipeEnabledProperty(); } },
          { L"candragitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::CanDragItemsProperty(); } },
          { L"canreorderitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::CanReorderItemsProperty(); } },
          { L"isitemclickenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::IsItemClickEnabledProperty(); } },
          { L"datafetchsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::DataFetchSizeProperty(); } },
          { L"incrementalloadingthreshold"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::IncrementalLoadingThresholdProperty(); } },
          { L"incrementalloadingtrigger"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::IncrementalLoadingTriggerProperty(); } },
          { L"showsscrollingplaceholders"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::ShowsScrollingPlaceholdersProperty(); } },
          { L"reordermode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::ReorderModeProperty(); } },
          { L"ismultiselectcheckboxenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::IsMultiSelectCheckBoxEnabledProperty(); } },
          { L"singleselectionfollowsfocus"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::SingleSelectionFollowsFocusProperty(); } },
          { L"semanticzoomowner"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::SemanticZoomOwnerProperty(); } },
          { L"isactiveview"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::IsActiveViewProperty(); } },
          { L"iszoomedinview"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::IsZoomedInViewProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::HeaderTemplateProperty(); } },
          { L"headertransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::HeaderTransitionsProperty(); } },
          { L"footer"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::FooterProperty(); } },
          { L"footertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::FooterTemplateProperty(); } },
          { L"footertransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ListViewBase::FooterTransitionsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MapControl::LayersProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MapControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"layers"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MapControl::LayersProperty(); } },
          { L"mapservicetoken"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MapControl::MapServiceTokenProperty(); } },
          { L"center"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MapControl::CenterProperty(); } },
          { L"zoomlevel"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MapControl::ZoomLevelProperty(); } },
          { L"interactivecontrolsvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MapControl::InteractiveControlsVisibleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MapElementsLayer::MapElementsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MapElementsLayer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"mapelements"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MapElementsLayer::MapElementsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MapIcon::LocationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MapIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"location"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MapIcon::LocationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MediaPlayerElement::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MediaPlayerElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerElement::SourceProperty(); } },
          { L"aretransportcontrolsenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerElement::AreTransportControlsEnabledProperty(); } },
          { L"postersource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerElement::PosterSourceProperty(); } },
          { L"stretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerElement::StretchProperty(); } },
          { L"autoplay"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerElement::AutoPlayProperty(); } },
          { L"isfullwindow"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerElement::IsFullWindowProperty(); } },
          { L"mediaplayer"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerElement::MediaPlayerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MediaPlayerPresenter::MediaPlayerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MediaPlayerPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"mediaplayer"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerPresenter::MediaPlayerProperty(); } },
          { L"stretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerPresenter::StretchProperty(); } },
          { L"isfullwindow"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaPlayerPresenter::IsFullWindowProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsZoomButtonVisibleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iszoombuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsZoomButtonVisibleProperty(); } },
          { L"iszoomenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsZoomEnabledProperty(); } },
          { L"isfastforwardbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsFastForwardButtonVisibleProperty(); } },
          { L"isfastforwardenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsFastForwardEnabledProperty(); } },
          { L"isfastrewindbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsFastRewindButtonVisibleProperty(); } },
          { L"isfastrewindenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsFastRewindEnabledProperty(); } },
          { L"isstopbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsStopButtonVisibleProperty(); } },
          { L"isstopenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsStopEnabledProperty(); } },
          { L"isvolumebuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsVolumeButtonVisibleProperty(); } },
          { L"isvolumeenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsVolumeEnabledProperty(); } },
          { L"isplaybackratebuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsPlaybackRateButtonVisibleProperty(); } },
          { L"isplaybackrateenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsPlaybackRateEnabledProperty(); } },
          { L"isseekbarvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsSeekBarVisibleProperty(); } },
          { L"isseekenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsSeekEnabledProperty(); } },
          { L"iscompact"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsCompactProperty(); } },
          { L"isskipforwardbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsSkipForwardButtonVisibleProperty(); } },
          { L"isskipforwardenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsSkipForwardEnabledProperty(); } },
          { L"isskipbackwardbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsSkipBackwardButtonVisibleProperty(); } },
          { L"isskipbackwardenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsSkipBackwardEnabledProperty(); } },
          { L"isnexttrackbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsNextTrackButtonVisibleProperty(); } },
          { L"isprevioustrackbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsPreviousTrackButtonVisibleProperty(); } },
          { L"fastplayfallbackbehaviour"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::FastPlayFallbackBehaviourProperty(); } },
          { L"showandhideautomatically"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::ShowAndHideAutomaticallyProperty(); } },
          { L"isrepeatenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsRepeatEnabledProperty(); } },
          { L"isrepeatbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControls::IsRepeatButtonVisibleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MediaTransportControlsHelper::DropoutOrderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MediaTransportControlsHelper>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"dropoutorder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MediaTransportControlsHelper::DropoutOrderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MenuBar::ItemsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MenuBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"items"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuBar::ItemsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MenuBarItem::TitleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MenuBarItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"title"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuBarItem::TitleProperty(); } },
          { L"items"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuBarItem::ItemsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MenuFlyout::MenuFlyoutPresenterStyleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MenuFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"menuflyoutpresenterstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyout::MenuFlyoutPresenterStyleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem::TextProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"text"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem::TextProperty(); } },
          { L"command"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem::CommandProperty(); } },
          { L"commandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem::CommandParameterProperty(); } },
          { L"icon"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem::IconProperty(); } },
          { L"keyboardacceleratortextoverride"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem::KeyboardAcceleratorTextOverrideProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutPresenter::IsDefaultShadowEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdefaultshadowenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutPresenter::IsDefaultShadowEnabledProperty(); } },
          { L"systembackdrop"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutPresenter::SystemBackdropProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem::TextProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"text"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem::TextProperty(); } },
          { L"icon"s, []() { return winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem::IconProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::NavigationView::IsPaneOpenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::NavigationView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ispaneopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::IsPaneOpenProperty(); } },
          { L"compactmodethresholdwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::CompactModeThresholdWidthProperty(); } },
          { L"expandedmodethresholdwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::ExpandedModeThresholdWidthProperty(); } },
          { L"footermenuitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::FooterMenuItemsProperty(); } },
          { L"footermenuitemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::FooterMenuItemsSourceProperty(); } },
          { L"panefooter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::PaneFooterProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::HeaderTemplateProperty(); } },
          { L"displaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::DisplayModeProperty(); } },
          { L"issettingsvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::IsSettingsVisibleProperty(); } },
          { L"ispanetogglebuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::IsPaneToggleButtonVisibleProperty(); } },
          { L"alwaysshowheader"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::AlwaysShowHeaderProperty(); } },
          { L"compactpanelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::CompactPaneLengthProperty(); } },
          { L"openpanelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::OpenPaneLengthProperty(); } },
          { L"panetogglebuttonstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::PaneToggleButtonStyleProperty(); } },
          { L"menuitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::MenuItemsProperty(); } },
          { L"menuitemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::MenuItemsSourceProperty(); } },
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::SelectedItemProperty(); } },
          { L"settingsitem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::SettingsItemProperty(); } },
          { L"autosuggestbox"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::AutoSuggestBoxProperty(); } },
          { L"menuitemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::MenuItemTemplateProperty(); } },
          { L"menuitemtemplateselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::MenuItemTemplateSelectorProperty(); } },
          { L"menuitemcontainerstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::MenuItemContainerStyleProperty(); } },
          { L"menuitemcontainerstyleselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::MenuItemContainerStyleSelectorProperty(); } },
          { L"istitlebarautopaddingenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::IsTitleBarAutoPaddingEnabledProperty(); } },
          { L"isbackbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::IsBackButtonVisibleProperty(); } },
          { L"isbackenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::IsBackEnabledProperty(); } },
          { L"panetitle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::PaneTitleProperty(); } },
          { L"panedisplaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::PaneDisplayModeProperty(); } },
          { L"paneheader"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::PaneHeaderProperty(); } },
          { L"panecustomcontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::PaneCustomContentProperty(); } },
          { L"contentoverlay"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::ContentOverlayProperty(); } },
          { L"ispanevisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::IsPaneVisibleProperty(); } },
          { L"selectionfollowsfocus"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::SelectionFollowsFocusProperty(); } },
          { L"templatesettings"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::TemplateSettingsProperty(); } },
          { L"shouldernavigationenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::ShoulderNavigationEnabledProperty(); } },
          { L"overflowlabelmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationView::OverflowLabelModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::IconProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"icon"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::IconProperty(); } },
          { L"compactpanelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::CompactPaneLengthProperty(); } },
          { L"selectsoninvoked"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::SelectsOnInvokedProperty(); } },
          { L"isexpanded"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::IsExpandedProperty(); } },
          { L"hasunrealizedchildren"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::HasUnrealizedChildrenProperty(); } },
          { L"ischildselected"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::IsChildSelectedProperty(); } },
          { L"menuitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::MenuItemsProperty(); } },
          { L"menuitemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::MenuItemsSourceProperty(); } },
          { L"infobadge"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem::InfoBadgeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemBase::IsSelectedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isselected"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemBase::IsSelectedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::TopPaddingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"toppadding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::TopPaddingProperty(); } },
          { L"overflowbuttonvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::OverflowButtonVisibilityProperty(); } },
          { L"panetogglebuttonvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::PaneToggleButtonVisibilityProperty(); } },
          { L"backbuttonvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::BackButtonVisibilityProperty(); } },
          { L"toppanevisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::TopPaneVisibilityProperty(); } },
          { L"leftpanevisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::LeftPaneVisibilityProperty(); } },
          { L"singleselectionfollowsfocus"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::SingleSelectionFollowsFocusProperty(); } },
          { L"panetogglebuttonwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::PaneToggleButtonWidthProperty(); } },
          { L"smallerpanetogglebuttonwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::SmallerPaneToggleButtonWidthProperty(); } },
          { L"openpanelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NavigationViewTemplateSettings::OpenPaneLengthProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::NumberBox::MinimumProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::NumberBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"minimum"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::MinimumProperty(); } },
          { L"maximum"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::MaximumProperty(); } },
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::ValueProperty(); } },
          { L"smallchange"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::SmallChangeProperty(); } },
          { L"largechange"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::LargeChangeProperty(); } },
          { L"text"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::TextProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::HeaderTemplateProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::PlaceholderTextProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::SelectionFlyoutProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::SelectionHighlightColorProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::TextReadingOrderProperty(); } },
          { L"preventkeyboarddisplayonprogrammaticfocus"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::PreventKeyboardDisplayOnProgrammaticFocusProperty(); } },
          { L"description"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::DescriptionProperty(); } },
          { L"validationmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::ValidationModeProperty(); } },
          { L"spinbuttonplacementmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::SpinButtonPlacementModeProperty(); } },
          { L"iswrapenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::IsWrapEnabledProperty(); } },
          { L"acceptsexpression"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::AcceptsExpressionProperty(); } },
          { L"numberformatter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::NumberBox::NumberFormatterProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Page::FrameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Page>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"frame"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Page::FrameProperty(); } },
          { L"topappbar"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Page::TopAppBarProperty(); } },
          { L"bottomappbar"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Page::BottomAppBarProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Panel::BackgroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Panel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"background"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Panel::BackgroundProperty(); } },
          { L"isitemshost"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Panel::IsItemsHostProperty(); } },
          { L"childrentransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Panel::ChildrenTransitionsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ParallaxView::ChildProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ParallaxView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"child"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::ChildProperty(); } },
          { L"horizontalsourceendoffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::HorizontalSourceEndOffsetProperty(); } },
          { L"horizontalsourceoffsetkind"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::HorizontalSourceOffsetKindProperty(); } },
          { L"horizontalsourcestartoffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::HorizontalSourceStartOffsetProperty(); } },
          { L"maxhorizontalshiftratio"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::MaxHorizontalShiftRatioProperty(); } },
          { L"horizontalshift"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::HorizontalShiftProperty(); } },
          { L"ishorizontalshiftclamped"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::IsHorizontalShiftClampedProperty(); } },
          { L"isverticalshiftclamped"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::IsVerticalShiftClampedProperty(); } },
          { L"source"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::SourceProperty(); } },
          { L"verticalsourceendoffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::VerticalSourceEndOffsetProperty(); } },
          { L"verticalsourceoffsetkind"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::VerticalSourceOffsetKindProperty(); } },
          { L"verticalsourcestartoffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::VerticalSourceStartOffsetProperty(); } },
          { L"maxverticalshiftratio"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::MaxVerticalShiftRatioProperty(); } },
          { L"verticalshift"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ParallaxView::VerticalShiftProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::PasswordBox::PasswordProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::PasswordBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"password"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::PasswordProperty(); } },
          { L"passwordchar"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::PasswordCharProperty(); } },
          { L"ispasswordrevealbuttonenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::IsPasswordRevealButtonEnabledProperty(); } },
          { L"maxlength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::MaxLengthProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::HeaderTemplateProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::PlaceholderTextProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::SelectionHighlightColorProperty(); } },
          { L"preventkeyboarddisplayonprogrammaticfocus"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::PreventKeyboardDisplayOnProgrammaticFocusProperty(); } },
          { L"passwordrevealmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::PasswordRevealModeProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::TextReadingOrderProperty(); } },
          { L"inputscope"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::InputScopeProperty(); } },
          { L"canpasteclipboardcontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::CanPasteClipboardContentProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::SelectionFlyoutProperty(); } },
          { L"description"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PasswordBox::DescriptionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::PathIcon::DataProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::PathIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"data"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PathIcon::DataProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::PathIconSource::DataProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::PathIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"data"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PathIconSource::DataProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::PersonPicture::BadgeNumberProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::PersonPicture>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"badgenumber"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::BadgeNumberProperty(); } },
          { L"badgeglyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::BadgeGlyphProperty(); } },
          { L"badgeimagesource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::BadgeImageSourceProperty(); } },
          { L"badgetext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::BadgeTextProperty(); } },
          { L"isgroup"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::IsGroupProperty(); } },
          { L"contact"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::ContactProperty(); } },
          { L"displayname"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::DisplayNameProperty(); } },
          { L"initials"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::InitialsProperty(); } },
          { L"prefersmallimage"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::PreferSmallImageProperty(); } },
          { L"profilepicture"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PersonPicture::ProfilePictureProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::PickerFlyout::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::PickerFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PickerFlyout::ContentProperty(); } },
          { L"confirmationbuttonsvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PickerFlyout::ConfirmationButtonsVisibleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::PipsPager::NumberOfPagesProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::PipsPager>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"numberofpages"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::NumberOfPagesProperty(); } },
          { L"selectedpageindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::SelectedPageIndexProperty(); } },
          { L"maxvisiblepips"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::MaxVisiblePipsProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::OrientationProperty(); } },
          { L"previousbuttonvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::PreviousButtonVisibilityProperty(); } },
          { L"nextbuttonvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::NextButtonVisibilityProperty(); } },
          { L"previousbuttonstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::PreviousButtonStyleProperty(); } },
          { L"nextbuttonstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::NextButtonStyleProperty(); } },
          { L"selectedpipstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::SelectedPipStyleProperty(); } },
          { L"normalpipstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::NormalPipStyleProperty(); } },
          { L"wrapmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PipsPager::WrapModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Pivot::TitleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Pivot>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"title"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::TitleProperty(); } },
          { L"titletemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::TitleTemplateProperty(); } },
          { L"leftheader"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::LeftHeaderProperty(); } },
          { L"leftheadertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::LeftHeaderTemplateProperty(); } },
          { L"rightheader"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::RightHeaderProperty(); } },
          { L"rightheadertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::RightHeaderTemplateProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::HeaderTemplateProperty(); } },
          { L"selectedindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::SelectedItemProperty(); } },
          { L"islocked"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::IsLockedProperty(); } },
          { L"headerfocusvisualplacement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::HeaderFocusVisualPlacementProperty(); } },
          { L"isheaderitemscarouselenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::IsHeaderItemsCarouselEnabledProperty(); } },
          { L"slideinanimationgroup"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Pivot::SlideInAnimationGroupProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::PivotItem::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::PivotItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::PivotItem::HeaderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ProgressBar::IsIndeterminateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ProgressBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isindeterminate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ProgressBar::IsIndeterminateProperty(); } },
          { L"showerror"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ProgressBar::ShowErrorProperty(); } },
          { L"showpaused"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ProgressBar::ShowPausedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ProgressRing::IsActiveProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ProgressRing>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isactive"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ProgressRing::IsActiveProperty(); } },
          { L"isindeterminate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ProgressRing::IsIndeterminateProperty(); } },
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ProgressRing::ValueProperty(); } },
          { L"minimum"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ProgressRing::MinimumProperty(); } },
          { L"maximum"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ProgressRing::MaximumProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RadioButton::GroupNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RadioButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"groupname"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioButton::GroupNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RadioButtons::ItemsSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RadioButtons>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioButtons::ItemsSourceProperty(); } },
          { L"items"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioButtons::ItemsProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioButtons::ItemTemplateProperty(); } },
          { L"selectedindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioButtons::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioButtons::SelectedItemProperty(); } },
          { L"maxcolumns"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioButtons::MaxColumnsProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioButtons::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioButtons::HeaderTemplateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RadioMenuFlyoutItem::IsCheckedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RadioMenuFlyoutItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ischecked"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioMenuFlyoutItem::IsCheckedProperty(); } },
          { L"groupname"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioMenuFlyoutItem::GroupNameProperty(); } },
          { L"arecheckstatesenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RadioMenuFlyoutItem::AreCheckStatesEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RatingControl::CaptionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RatingControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"caption"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingControl::CaptionProperty(); } },
          { L"initialsetvalue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingControl::InitialSetValueProperty(); } },
          { L"isclearenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingControl::IsClearEnabledProperty(); } },
          { L"isreadonly"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingControl::IsReadOnlyProperty(); } },
          { L"maxrating"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingControl::MaxRatingProperty(); } },
          { L"placeholdervalue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingControl::PlaceholderValueProperty(); } },
          { L"iteminfo"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingControl::ItemInfoProperty(); } },
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingControl::ValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RatingItemFontInfo::DisabledGlyphProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RatingItemFontInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"disabledglyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemFontInfo::DisabledGlyphProperty(); } },
          { L"glyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemFontInfo::GlyphProperty(); } },
          { L"placeholderglyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemFontInfo::PlaceholderGlyphProperty(); } },
          { L"pointeroverglyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemFontInfo::PointerOverGlyphProperty(); } },
          { L"pointeroverplaceholderglyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemFontInfo::PointerOverPlaceholderGlyphProperty(); } },
          { L"unsetglyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemFontInfo::UnsetGlyphProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RatingItemImageInfo::DisabledImageProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RatingItemImageInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"disabledimage"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemImageInfo::DisabledImageProperty(); } },
          { L"image"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemImageInfo::ImageProperty(); } },
          { L"placeholderimage"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemImageInfo::PlaceholderImageProperty(); } },
          { L"pointeroverimage"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemImageInfo::PointerOverImageProperty(); } },
          { L"pointeroverplaceholderimage"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemImageInfo::PointerOverPlaceholderImageProperty(); } },
          { L"unsetimage"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RatingItemImageInfo::UnsetImageProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RefreshContainer::VisualizerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RefreshContainer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"visualizer"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RefreshContainer::VisualizerProperty(); } },
          { L"pulldirection"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RefreshContainer::PullDirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RefreshVisualizer::InfoProviderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RefreshVisualizer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"infoprovider"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RefreshVisualizer::InfoProviderProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RefreshVisualizer::OrientationProperty(); } },
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RefreshVisualizer::ContentProperty(); } },
          { L"state"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RefreshVisualizer::StateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RelativePanel::BackgroundSizingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RelativePanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"backgroundsizing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::BackgroundSizingProperty(); } },
          { L"leftof"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::LeftOfProperty(); } },
          { L"above"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AboveProperty(); } },
          { L"rightof"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::RightOfProperty(); } },
          { L"below"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::BelowProperty(); } },
          { L"alignhorizontalcenterwith"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignHorizontalCenterWithProperty(); } },
          { L"alignverticalcenterwith"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignVerticalCenterWithProperty(); } },
          { L"alignleftwith"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignLeftWithProperty(); } },
          { L"aligntopwith"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignTopWithProperty(); } },
          { L"alignrightwith"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignRightWithProperty(); } },
          { L"alignbottomwith"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignBottomWithProperty(); } },
          { L"alignleftwithpanel"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignLeftWithPanelProperty(); } },
          { L"aligntopwithpanel"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignTopWithPanelProperty(); } },
          { L"alignrightwithpanel"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignRightWithPanelProperty(); } },
          { L"alignbottomwithpanel"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignBottomWithPanelProperty(); } },
          { L"alignhorizontalcenterwithpanel"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignHorizontalCenterWithPanelProperty(); } },
          { L"alignverticalcenterwithpanel"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::AlignVerticalCenterWithPanelProperty(); } },
          { L"borderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::BorderThicknessProperty(); } },
          { L"cornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RelativePanel::PaddingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RichEditBox::IsReadOnlyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RichEditBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isreadonly"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::IsReadOnlyProperty(); } },
          { L"acceptsreturn"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::AcceptsReturnProperty(); } },
          { L"textalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::TextAlignmentProperty(); } },
          { L"textwrapping"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::TextWrappingProperty(); } },
          { L"isspellcheckenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::IsSpellCheckEnabledProperty(); } },
          { L"istextpredictionenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::IsTextPredictionEnabledProperty(); } },
          { L"inputscope"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::InputScopeProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::HeaderTemplateProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::PlaceholderTextProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::SelectionHighlightColorProperty(); } },
          { L"preventkeyboarddisplayonprogrammaticfocus"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::PreventKeyboardDisplayOnProgrammaticFocusProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::IsColorFontEnabledProperty(); } },
          { L"selectionhighlightcolorwhennotfocused"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::SelectionHighlightColorWhenNotFocusedProperty(); } },
          { L"maxlength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::MaxLengthProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::HorizontalTextAlignmentProperty(); } },
          { L"charactercasing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::CharacterCasingProperty(); } },
          { L"disabledformattingaccelerators"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::DisabledFormattingAcceleratorsProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::SelectionFlyoutProperty(); } },
          { L"proofingmenuflyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::ProofingMenuFlyoutProperty(); } },
          { L"description"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::DescriptionProperty(); } },
          { L"desiredcandidatewindowalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::DesiredCandidateWindowAlignmentProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::TextReadingOrderProperty(); } },
          { L"clipboardcopyformat"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichEditBox::ClipboardCopyFormatProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::FontSizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RichTextBlock>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::FontStretchProperty(); } },
          { L"foreground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::ForegroundProperty(); } },
          { L"textwrapping"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::TextWrappingProperty(); } },
          { L"texttrimming"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::TextTrimmingProperty(); } },
          { L"textalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::TextAlignmentProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::PaddingProperty(); } },
          { L"lineheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::LineHeightProperty(); } },
          { L"linestackingstrategy"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::LineStackingStrategyProperty(); } },
          { L"characterspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::CharacterSpacingProperty(); } },
          { L"overflowcontenttarget"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::OverflowContentTargetProperty(); } },
          { L"istextselectionenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::IsTextSelectionEnabledProperty(); } },
          { L"hasoverflowcontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::HasOverflowContentProperty(); } },
          { L"selectedtext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::SelectedTextProperty(); } },
          { L"maxlines"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::MaxLinesProperty(); } },
          { L"textlinebounds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::TextLineBoundsProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::SelectionHighlightColorProperty(); } },
          { L"opticalmarginalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::OpticalMarginAlignmentProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::IsColorFontEnabledProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::TextReadingOrderProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::IsTextScaleFactorEnabledProperty(); } },
          { L"textdecorations"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::TextDecorationsProperty(); } },
          { L"istexttrimmed"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::IsTextTrimmedProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::HorizontalTextAlignmentProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::SelectionFlyoutProperty(); } },
          { L"textindent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlock::TextIndentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RichTextBlockOverflow::OverflowContentTargetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RichTextBlockOverflow>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"overflowcontenttarget"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlockOverflow::OverflowContentTargetProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlockOverflow::PaddingProperty(); } },
          { L"hasoverflowcontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlockOverflow::HasOverflowContentProperty(); } },
          { L"maxlines"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlockOverflow::MaxLinesProperty(); } },
          { L"istexttrimmed"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RichTextBlockOverflow::IsTextTrimmedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::RowDefinition::HeightProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::RowDefinition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"height"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RowDefinition::HeightProperty(); } },
          { L"maxheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RowDefinition::MaxHeightProperty(); } },
          { L"minheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::RowDefinition::MinHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ScrollContentPresenter::CanContentRenderOutsideBoundsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ScrollContentPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"cancontentrenderoutsidebounds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollContentPresenter::CanContentRenderOutsideBoundsProperty(); } },
          { L"sizescontenttotemplatedparent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollContentPresenter::SizesContentToTemplatedParentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ScrollView::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ScrollView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::ContentProperty(); } },
          { L"scrollpresenter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::ScrollPresenterProperty(); } },
          { L"horizontalscrollbarvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::HorizontalScrollBarVisibilityProperty(); } },
          { L"verticalscrollbarvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::VerticalScrollBarVisibilityProperty(); } },
          { L"contentorientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::ContentOrientationProperty(); } },
          { L"horizontalscrollchainmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::HorizontalScrollChainModeProperty(); } },
          { L"verticalscrollchainmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::VerticalScrollChainModeProperty(); } },
          { L"horizontalscrollrailmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::HorizontalScrollRailModeProperty(); } },
          { L"verticalscrollrailmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::VerticalScrollRailModeProperty(); } },
          { L"horizontalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::HorizontalScrollModeProperty(); } },
          { L"verticalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::VerticalScrollModeProperty(); } },
          { L"computedhorizontalscrollbarvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::ComputedHorizontalScrollBarVisibilityProperty(); } },
          { L"computedverticalscrollbarvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::ComputedVerticalScrollBarVisibilityProperty(); } },
          { L"computedhorizontalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::ComputedHorizontalScrollModeProperty(); } },
          { L"computedverticalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::ComputedVerticalScrollModeProperty(); } },
          { L"zoomchainmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::ZoomChainModeProperty(); } },
          { L"zoommode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::ZoomModeProperty(); } },
          { L"ignoredinputkinds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::IgnoredInputKindsProperty(); } },
          { L"minzoomfactor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::MinZoomFactorProperty(); } },
          { L"maxzoomfactor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::MaxZoomFactorProperty(); } },
          { L"horizontalanchorratio"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::HorizontalAnchorRatioProperty(); } },
          { L"verticalanchorratio"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollView::VerticalAnchorRatioProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::HorizontalSnapPointsAlignmentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ScrollViewer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"horizontalsnappointsalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::HorizontalSnapPointsAlignmentProperty(); } },
          { L"verticalsnappointsalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::VerticalSnapPointsAlignmentProperty(); } },
          { L"horizontalsnappointstype"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::HorizontalSnapPointsTypeProperty(); } },
          { L"verticalsnappointstype"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::VerticalSnapPointsTypeProperty(); } },
          { L"zoomsnappointstype"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ZoomSnapPointsTypeProperty(); } },
          { L"horizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::HorizontalOffsetProperty(); } },
          { L"viewportwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ViewportWidthProperty(); } },
          { L"scrollablewidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ScrollableWidthProperty(); } },
          { L"computedhorizontalscrollbarvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ComputedHorizontalScrollBarVisibilityProperty(); } },
          { L"extentwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ExtentWidthProperty(); } },
          { L"verticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::VerticalOffsetProperty(); } },
          { L"viewportheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ViewportHeightProperty(); } },
          { L"scrollableheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ScrollableHeightProperty(); } },
          { L"computedverticalscrollbarvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ComputedVerticalScrollBarVisibilityProperty(); } },
          { L"extentheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ExtentHeightProperty(); } },
          { L"minzoomfactor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::MinZoomFactorProperty(); } },
          { L"maxzoomfactor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::MaxZoomFactorProperty(); } },
          { L"zoomfactor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ZoomFactorProperty(); } },
          { L"zoomsnappoints"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ZoomSnapPointsProperty(); } },
          { L"topleftheader"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::TopLeftHeaderProperty(); } },
          { L"leftheader"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::LeftHeaderProperty(); } },
          { L"topheader"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::TopHeaderProperty(); } },
          { L"reduceviewportforcoreinputviewocclusions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ReduceViewportForCoreInputViewOcclusionsProperty(); } },
          { L"horizontalanchorratio"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::HorizontalAnchorRatioProperty(); } },
          { L"verticalanchorratio"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::VerticalAnchorRatioProperty(); } },
          { L"horizontalscrollbarvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::HorizontalScrollBarVisibilityProperty(); } },
          { L"verticalscrollbarvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::VerticalScrollBarVisibilityProperty(); } },
          { L"ishorizontalrailenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::IsHorizontalRailEnabledProperty(); } },
          { L"isverticalrailenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::IsVerticalRailEnabledProperty(); } },
          { L"ishorizontalscrollchainingenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::IsHorizontalScrollChainingEnabledProperty(); } },
          { L"isverticalscrollchainingenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::IsVerticalScrollChainingEnabledProperty(); } },
          { L"iszoomchainingenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::IsZoomChainingEnabledProperty(); } },
          { L"isscrollinertiaenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::IsScrollInertiaEnabledProperty(); } },
          { L"iszoominertiaenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::IsZoomInertiaEnabledProperty(); } },
          { L"horizontalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::HorizontalScrollModeProperty(); } },
          { L"verticalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::VerticalScrollModeProperty(); } },
          { L"zoommode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::ZoomModeProperty(); } },
          { L"cancontentrenderoutsidebounds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::CanContentRenderOutsideBoundsProperty(); } },
          { L"isdeferredscrollingenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::IsDeferredScrollingEnabledProperty(); } },
          { L"bringintoviewonfocuschange"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ScrollViewer::BringIntoViewOnFocusChangeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SelectorBar::ItemsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SelectorBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"items"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SelectorBar::ItemsProperty(); } },
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SelectorBar::SelectedItemProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SelectorBarItem::TextProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SelectorBarItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"text"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SelectorBarItem::TextProperty(); } },
          { L"icon"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SelectorBarItem::IconProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SemanticZoom::ZoomedInViewProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SemanticZoom>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"zoomedinview"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SemanticZoom::ZoomedInViewProperty(); } },
          { L"zoomedoutview"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SemanticZoom::ZoomedOutViewProperty(); } },
          { L"iszoomedinviewactive"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SemanticZoom::IsZoomedInViewActiveProperty(); } },
          { L"canchangeviews"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SemanticZoom::CanChangeViewsProperty(); } },
          { L"iszoomoutbuttonenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SemanticZoom::IsZoomOutButtonEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Slider::IntermediateValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Slider>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"intermediatevalue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::IntermediateValueProperty(); } },
          { L"stepfrequency"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::StepFrequencyProperty(); } },
          { L"snapsto"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::SnapsToProperty(); } },
          { L"tickfrequency"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::TickFrequencyProperty(); } },
          { L"tickplacement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::TickPlacementProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::OrientationProperty(); } },
          { L"isdirectionreversed"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::IsDirectionReversedProperty(); } },
          { L"isthumbtooltipenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::IsThumbToolTipEnabledProperty(); } },
          { L"thumbtooltipvalueconverter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::ThumbToolTipValueConverterProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Slider::HeaderTemplateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SplitButton::FlyoutProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SplitButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"flyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitButton::FlyoutProperty(); } },
          { L"command"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitButton::CommandProperty(); } },
          { L"commandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitButton::CommandParameterProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SplitView::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SplitView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::ContentProperty(); } },
          { L"pane"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::PaneProperty(); } },
          { L"ispaneopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::IsPaneOpenProperty(); } },
          { L"openpanelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::OpenPaneLengthProperty(); } },
          { L"compactpanelength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::CompactPaneLengthProperty(); } },
          { L"paneplacement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::PanePlacementProperty(); } },
          { L"displaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::DisplayModeProperty(); } },
          { L"templatesettings"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::TemplateSettingsProperty(); } },
          { L"panebackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::PaneBackgroundProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SplitView::LightDismissOverlayModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::StackLayout::OrientationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::StackLayout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackLayout::OrientationProperty(); } },
          { L"spacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackLayout::SpacingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::StackPanel::AreScrollSnapPointsRegularProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::StackPanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"arescrollsnappointsregular"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackPanel::AreScrollSnapPointsRegularProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackPanel::OrientationProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackPanel::BackgroundSizingProperty(); } },
          { L"borderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackPanel::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackPanel::BorderThicknessProperty(); } },
          { L"cornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackPanel::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackPanel::PaddingProperty(); } },
          { L"spacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::StackPanel::SpacingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel::CompositionScaleXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"compositionscalex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel::CompositionScaleXProperty(); } },
          { L"compositionscaley"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel::CompositionScaleYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SwipeControl::LeftItemsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SwipeControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"leftitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeControl::LeftItemsProperty(); } },
          { L"rightitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeControl::RightItemsProperty(); } },
          { L"topitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeControl::TopItemsProperty(); } },
          { L"bottomitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeControl::BottomItemsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SwipeItem::IconSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SwipeItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iconsource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeItem::IconSourceProperty(); } },
          { L"text"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeItem::TextProperty(); } },
          { L"background"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeItem::BackgroundProperty(); } },
          { L"foreground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeItem::ForegroundProperty(); } },
          { L"command"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeItem::CommandProperty(); } },
          { L"commandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeItem::CommandParameterProperty(); } },
          { L"behavioroninvoked"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeItem::BehaviorOnInvokedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SwipeItems::ModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SwipeItems>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"mode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SwipeItems::ModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SymbolIcon::SymbolProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SymbolIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"symbol"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SymbolIcon::SymbolProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::SymbolIconSource::SymbolProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::SymbolIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"symbol"s, []() { return winrt::Microsoft::UI::Xaml::Controls::SymbolIconSource::SymbolProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TabView::TabWidthModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TabView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"tabwidthmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::TabWidthModeProperty(); } },
          { L"tabstripheader"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::TabStripHeaderProperty(); } },
          { L"tabstripheadertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::TabStripHeaderTemplateProperty(); } },
          { L"tabstripfooter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::TabStripFooterProperty(); } },
          { L"tabstripfootertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::TabStripFooterTemplateProperty(); } },
          { L"isaddtabbuttonvisible"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::IsAddTabButtonVisibleProperty(); } },
          { L"addtabbuttoncommand"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::AddTabButtonCommandProperty(); } },
          { L"addtabbuttoncommandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::AddTabButtonCommandParameterProperty(); } },
          { L"tabitemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::TabItemsSourceProperty(); } },
          { L"tabitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::TabItemsProperty(); } },
          { L"tabitemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::TabItemTemplateProperty(); } },
          { L"tabitemtemplateselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::TabItemTemplateSelectorProperty(); } },
          { L"closebuttonoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::CloseButtonOverlayModeProperty(); } },
          { L"candragtabs"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::CanDragTabsProperty(); } },
          { L"canreordertabs"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::CanReorderTabsProperty(); } },
          { L"allowdroptabs"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::AllowDropTabsProperty(); } },
          { L"selectedindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::SelectedItemProperty(); } },
          { L"cantearouttabs"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabView::CanTearOutTabsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TabViewItem::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TabViewItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabViewItem::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabViewItem::HeaderTemplateProperty(); } },
          { L"iconsource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabViewItem::IconSourceProperty(); } },
          { L"isclosable"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabViewItem::IsClosableProperty(); } },
          { L"tabviewtemplatesettings"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabViewItem::TabViewTemplateSettingsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TabViewItemTemplateSettings::IconElementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TabViewItemTemplateSettings>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iconelement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabViewItemTemplateSettings::IconElementProperty(); } },
          { L"tabgeometry"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TabViewItemTemplateSettings::TabGeometryProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TeachingTip::IsOpenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TeachingTip>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::IsOpenProperty(); } },
          { L"target"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::TargetProperty(); } },
          { L"tailvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::TailVisibilityProperty(); } },
          { L"title"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::TitleProperty(); } },
          { L"subtitle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::SubtitleProperty(); } },
          { L"actionbuttoncontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::ActionButtonContentProperty(); } },
          { L"actionbuttonstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::ActionButtonStyleProperty(); } },
          { L"actionbuttoncommand"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::ActionButtonCommandProperty(); } },
          { L"actionbuttoncommandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::ActionButtonCommandParameterProperty(); } },
          { L"closebuttoncontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::CloseButtonContentProperty(); } },
          { L"closebuttonstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::CloseButtonStyleProperty(); } },
          { L"closebuttoncommand"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::CloseButtonCommandProperty(); } },
          { L"closebuttoncommandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::CloseButtonCommandParameterProperty(); } },
          { L"placementmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::PlacementMarginProperty(); } },
          { L"shouldconstraintorootbounds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::ShouldConstrainToRootBoundsProperty(); } },
          { L"islightdismissenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::IsLightDismissEnabledProperty(); } },
          { L"preferredplacement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::PreferredPlacementProperty(); } },
          { L"herocontentplacement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::HeroContentPlacementProperty(); } },
          { L"herocontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::HeroContentProperty(); } },
          { L"iconsource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::IconSourceProperty(); } },
          { L"templatesettings"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTip::TemplateSettingsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TeachingTipTemplateSettings::TopRightHighlightMarginProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TeachingTipTemplateSettings>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"toprighthighlightmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTipTemplateSettings::TopRightHighlightMarginProperty(); } },
          { L"toplefthighlightmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTipTemplateSettings::TopLeftHighlightMarginProperty(); } },
          { L"iconelement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TeachingTipTemplateSettings::IconElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TextBlock::FontSizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TextBlock>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fontsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::FontStretchProperty(); } },
          { L"characterspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::CharacterSpacingProperty(); } },
          { L"foreground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::ForegroundProperty(); } },
          { L"textwrapping"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::TextWrappingProperty(); } },
          { L"texttrimming"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::TextTrimmingProperty(); } },
          { L"textalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::TextAlignmentProperty(); } },
          { L"text"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::TextProperty(); } },
          { L"padding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::PaddingProperty(); } },
          { L"lineheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::LineHeightProperty(); } },
          { L"linestackingstrategy"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::LineStackingStrategyProperty(); } },
          { L"istextselectionenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::IsTextSelectionEnabledProperty(); } },
          { L"selectedtext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::SelectedTextProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::SelectionHighlightColorProperty(); } },
          { L"maxlines"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::MaxLinesProperty(); } },
          { L"textlinebounds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::TextLineBoundsProperty(); } },
          { L"opticalmarginalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::OpticalMarginAlignmentProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::IsColorFontEnabledProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::TextReadingOrderProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::IsTextScaleFactorEnabledProperty(); } },
          { L"textdecorations"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::TextDecorationsProperty(); } },
          { L"istexttrimmed"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::IsTextTrimmedProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::HorizontalTextAlignmentProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBlock::SelectionFlyoutProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TextBox::TextProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TextBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"text"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::TextProperty(); } },
          { L"maxlength"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::MaxLengthProperty(); } },
          { L"isreadonly"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::IsReadOnlyProperty(); } },
          { L"acceptsreturn"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::AcceptsReturnProperty(); } },
          { L"textalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::TextAlignmentProperty(); } },
          { L"textwrapping"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::TextWrappingProperty(); } },
          { L"isspellcheckenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::IsSpellCheckEnabledProperty(); } },
          { L"istextpredictionenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::IsTextPredictionEnabledProperty(); } },
          { L"inputscope"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::InputScopeProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::HeaderTemplateProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::PlaceholderTextProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::SelectionHighlightColorProperty(); } },
          { L"preventkeyboarddisplayonprogrammaticfocus"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::PreventKeyboardDisplayOnProgrammaticFocusProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::IsColorFontEnabledProperty(); } },
          { L"selectionhighlightcolorwhennotfocused"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::SelectionHighlightColorWhenNotFocusedProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::HorizontalTextAlignmentProperty(); } },
          { L"charactercasing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::CharacterCasingProperty(); } },
          { L"placeholderforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::PlaceholderForegroundProperty(); } },
          { L"canpasteclipboardcontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::CanPasteClipboardContentProperty(); } },
          { L"canundo"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::CanUndoProperty(); } },
          { L"canredo"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::CanRedoProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::SelectionFlyoutProperty(); } },
          { L"proofingmenuflyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::ProofingMenuFlyoutProperty(); } },
          { L"description"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::DescriptionProperty(); } },
          { L"desiredcandidatewindowalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::DesiredCandidateWindowAlignmentProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TextBox::TextReadingOrderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TimePicker::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TimePicker>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePicker::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePicker::HeaderTemplateProperty(); } },
          { L"clockidentifier"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePicker::ClockIdentifierProperty(); } },
          { L"minuteincrement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePicker::MinuteIncrementProperty(); } },
          { L"time"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePicker::TimeProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePicker::LightDismissOverlayModeProperty(); } },
          { L"selectedtime"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePicker::SelectedTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TimePickerFlyout::ClockIdentifierProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TimePickerFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"clockidentifier"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePickerFlyout::ClockIdentifierProperty(); } },
          { L"time"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePickerFlyout::TimeProperty(); } },
          { L"minuteincrement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePickerFlyout::MinuteIncrementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TimePickerFlyoutPresenter::IsDefaultShadowEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TimePickerFlyoutPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdefaultshadowenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TimePickerFlyoutPresenter::IsDefaultShadowEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ToggleMenuFlyoutItem::IsCheckedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ToggleMenuFlyoutItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ischecked"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToggleMenuFlyoutItem::IsCheckedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ToggleSplitButton::IsCheckedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ToggleSplitButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ischecked"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToggleSplitButton::IsCheckedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch::IsOnProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ison"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch::IsOnProperty(); } },
          { L"header"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch::HeaderTemplateProperty(); } },
          { L"oncontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch::OnContentProperty(); } },
          { L"oncontenttemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch::OnContentTemplateProperty(); } },
          { L"offcontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch::OffContentProperty(); } },
          { L"offcontenttemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch::OffContentTemplateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ToolTip::HorizontalOffsetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ToolTip>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"horizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToolTip::HorizontalOffsetProperty(); } },
          { L"isopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToolTip::IsOpenProperty(); } },
          { L"placement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToolTip::PlacementProperty(); } },
          { L"placementtarget"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToolTip::PlacementTargetProperty(); } },
          { L"placementrect"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToolTip::PlacementRectProperty(); } },
          { L"verticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToolTip::VerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::ToolTipService::PlacementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::ToolTipService>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"placement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToolTipService::PlacementProperty(); } },
          { L"placementtarget"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToolTipService::PlacementTargetProperty(); } },
          { L"tooltip"s, []() { return winrt::Microsoft::UI::Xaml::Controls::ToolTipService::ToolTipProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TreeView::SelectedItemProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TreeView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::SelectedItemProperty(); } },
          { L"selectionmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::SelectionModeProperty(); } },
          { L"candragitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::CanDragItemsProperty(); } },
          { L"canreorderitems"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::CanReorderItemsProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::ItemTemplateProperty(); } },
          { L"itemtemplateselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::ItemTemplateSelectorProperty(); } },
          { L"itemcontainerstyle"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::ItemContainerStyleProperty(); } },
          { L"itemcontainerstyleselector"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::ItemContainerStyleSelectorProperty(); } },
          { L"itemcontainertransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::ItemContainerTransitionsProperty(); } },
          { L"itemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeView::ItemsSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::GlyphOpacityProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TreeViewItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"glyphopacity"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::GlyphOpacityProperty(); } },
          { L"glyphbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::GlyphBrushProperty(); } },
          { L"expandedglyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::ExpandedGlyphProperty(); } },
          { L"collapsedglyph"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::CollapsedGlyphProperty(); } },
          { L"glyphsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::GlyphSizeProperty(); } },
          { L"isexpanded"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::IsExpandedProperty(); } },
          { L"treeviewitemtemplatesettings"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::TreeViewItemTemplateSettingsProperty(); } },
          { L"hasunrealizedchildren"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::HasUnrealizedChildrenProperty(); } },
          { L"itemssource"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItem::ItemsSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TreeViewItemTemplateSettings::ExpandedGlyphVisibilityProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TreeViewItemTemplateSettings>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"expandedglyphvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItemTemplateSettings::ExpandedGlyphVisibilityProperty(); } },
          { L"collapsedglyphvisibility"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItemTemplateSettings::CollapsedGlyphVisibilityProperty(); } },
          { L"indentation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItemTemplateSettings::IndentationProperty(); } },
          { L"dragitemscount"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewItemTemplateSettings::DragItemsCountProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TreeViewNode::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TreeViewNode>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewNode::ContentProperty(); } },
          { L"depth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewNode::DepthProperty(); } },
          { L"isexpanded"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewNode::IsExpandedProperty(); } },
          { L"haschildren"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TreeViewNode::HasChildrenProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::Pane1Property())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::TwoPaneView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"pane1"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::Pane1Property(); } },
          { L"pane2"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::Pane2Property(); } },
          { L"pane1length"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::Pane1LengthProperty(); } },
          { L"pane2length"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::Pane2LengthProperty(); } },
          { L"panepriority"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::PanePriorityProperty(); } },
          { L"mode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::ModeProperty(); } },
          { L"widemodeconfiguration"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::WideModeConfigurationProperty(); } },
          { L"tallmodeconfiguration"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::TallModeConfigurationProperty(); } },
          { L"minwidemodewidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::MinWideModeWidthProperty(); } },
          { L"mintallmodeheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::TwoPaneView::MinTallModeHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout::OrientationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout::OrientationProperty(); } },
          { L"minitemwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout::MinItemWidthProperty(); } },
          { L"minitemheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout::MinItemHeightProperty(); } },
          { L"minrowspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout::MinRowSpacingProperty(); } },
          { L"mincolumnspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout::MinColumnSpacingProperty(); } },
          { L"itemsjustification"s, []() { return winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout::ItemsJustificationProperty(); } },
          { L"itemsstretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout::ItemsStretchProperty(); } },
          { L"maximumrowsorcolumns"s, []() { return winrt::Microsoft::UI::Xaml::Controls::UniformGridLayout::MaximumRowsOrColumnsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::UserControl::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::UserControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::UserControl::ContentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid::ItemHeightProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid::ItemHeightProperty(); } },
          { L"itemwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid::ItemWidthProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid::OrientationProperty(); } },
          { L"horizontalchildrenalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid::HorizontalChildrenAlignmentProperty(); } },
          { L"verticalchildrenalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid::VerticalChildrenAlignmentProperty(); } },
          { L"maximumrowsorcolumns"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid::MaximumRowsOrColumnsProperty(); } },
          { L"rowspan"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid::RowSpanProperty(); } },
          { L"columnspan"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VariableSizedWrapGrid::ColumnSpanProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Viewbox::StretchProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Viewbox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"stretch"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Viewbox::StretchProperty(); } },
          { L"stretchdirection"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Viewbox::StretchDirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::VirtualizingStackPanel::AreScrollSnapPointsRegularProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::VirtualizingStackPanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"arescrollsnappointsregular"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VirtualizingStackPanel::AreScrollSnapPointsRegularProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VirtualizingStackPanel::OrientationProperty(); } },
          { L"virtualizationmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VirtualizingStackPanel::VirtualizationModeProperty(); } },
          { L"isvirtualizing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::VirtualizingStackPanel::IsVirtualizingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::WebView2::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::WebView2>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WebView2::SourceProperty(); } },
          { L"cangoforward"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WebView2::CanGoForwardProperty(); } },
          { L"cangoback"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WebView2::CanGoBackProperty(); } },
          { L"defaultbackgroundcolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WebView2::DefaultBackgroundColorProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::WrapGrid::ItemWidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::WrapGrid>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WrapGrid::ItemWidthProperty(); } },
          { L"itemheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WrapGrid::ItemHeightProperty(); } },
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WrapGrid::OrientationProperty(); } },
          { L"horizontalchildrenalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WrapGrid::HorizontalChildrenAlignmentProperty(); } },
          { L"verticalchildrenalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WrapGrid::VerticalChildrenAlignmentProperty(); } },
          { L"maximumrowsorcolumns"s, []() { return winrt::Microsoft::UI::Xaml::Controls::WrapGrid::MaximumRowsOrColumnsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources::UseCompactResourcesProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"usecompactresources"s, []() { return winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources::UseCompactResourcesProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Controls.Primitives.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::AutoSuggestBoxHelper::KeepInteriorCornersSquareProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::AutoSuggestBoxHelper>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"keepinteriorcornerssquare"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::AutoSuggestBoxHelper::KeepInteriorCornersSquareProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::ButtonBase::ClickModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::ButtonBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"clickmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ButtonBase::ClickModeProperty(); } },
          { L"ispointerover"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ButtonBase::IsPointerOverProperty(); } },
          { L"ispressed"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ButtonBase::IsPressedProperty(); } },
          { L"command"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ButtonBase::CommandProperty(); } },
          { L"commandparameter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ButtonBase::CommandParameterProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorPickerSlider::ColorChannelProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorPickerSlider>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"colorchannel"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorPickerSlider::ColorChannelProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::ColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"color"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::ColorProperty(); } },
          { L"hsvcolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::HsvColorProperty(); } },
          { L"minhue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::MinHueProperty(); } },
          { L"maxhue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::MaxHueProperty(); } },
          { L"minsaturation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::MinSaturationProperty(); } },
          { L"maxsaturation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::MaxSaturationProperty(); } },
          { L"minvalue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::MinValueProperty(); } },
          { L"maxvalue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::MaxValueProperty(); } },
          { L"shape"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::ShapeProperty(); } },
          { L"components"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColorSpectrum::ComponentsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::ColumnMajorUniformToLargestGridLayout::MaxColumnsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::ColumnMajorUniformToLargestGridLayout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"maxcolumns"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColumnMajorUniformToLargestGridLayout::MaxColumnsProperty(); } },
          { L"columnspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColumnMajorUniformToLargestGridLayout::ColumnSpacingProperty(); } },
          { L"rowspacing"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ColumnMajorUniformToLargestGridLayout::RowSpacingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::ComboBoxHelper::KeepInteriorCornersSquareProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::ComboBoxHelper>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"keepinteriorcornerssquare"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ComboBoxHelper::KeepInteriorCornersSquareProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::CommandBarFlyoutCommandBar::SystemBackdropProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::CommandBarFlyoutCommandBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"systembackdrop"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::CommandBarFlyoutCommandBar::SystemBackdropProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::CornerRadiusFilterConverter::FilterProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::CornerRadiusFilterConverter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"filter"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::CornerRadiusFilterConverter::FilterProperty(); } },
          { L"scale"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::CornerRadiusFilterConverter::ScaleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::CornerRadiusToThicknessConverter::ConversionKindProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::CornerRadiusToThicknessConverter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"conversionkind"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::CornerRadiusToThicknessConverter::ConversionKindProperty(); } },
          { L"multiplier"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::CornerRadiusToThicknessConverter::MultiplierProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::TargetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"target"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::TargetProperty(); } },
          { L"placement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::PlacementProperty(); } },
          { L"allowfocusoninteraction"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::AllowFocusOnInteractionProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::LightDismissOverlayModeProperty(); } },
          { L"allowfocuswhendisabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::AllowFocusWhenDisabledProperty(); } },
          { L"showmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::ShowModeProperty(); } },
          { L"inputdeviceprefersprimarycommands"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::InputDevicePrefersPrimaryCommandsProperty(); } },
          { L"areopencloseanimationsenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::AreOpenCloseAnimationsEnabledProperty(); } },
          { L"shouldconstraintorootbounds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::ShouldConstrainToRootBoundsProperty(); } },
          { L"elementsoundmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::ElementSoundModeProperty(); } },
          { L"overlayinputpassthroughelement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::OverlayInputPassThroughElementProperty(); } },
          { L"isopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::IsOpenProperty(); } },
          { L"attachedflyout"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::AttachedFlyoutProperty(); } },
          { L"systembackdrop"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase::SystemBackdropProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectionCheckMarkVisualEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectioncheckmarkvisualenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectionCheckMarkVisualEnabledProperty(); } },
          { L"checkhintbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::CheckHintBrushProperty(); } },
          { L"checkselectingbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::CheckSelectingBrushProperty(); } },
          { L"checkbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::CheckBrushProperty(); } },
          { L"dragbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::DragBackgroundProperty(); } },
          { L"dragforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::DragForegroundProperty(); } },
          { L"focusborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::FocusBorderBrushProperty(); } },
          { L"placeholderbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::PlaceholderBackgroundProperty(); } },
          { L"pointeroverbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::PointerOverBackgroundProperty(); } },
          { L"selectedbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedBackgroundProperty(); } },
          { L"selectedforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedForegroundProperty(); } },
          { L"selectedpointeroverbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedPointerOverBackgroundProperty(); } },
          { L"selectedpointeroverborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedPointerOverBorderBrushProperty(); } },
          { L"selectedborderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedBorderThicknessProperty(); } },
          { L"disabledopacity"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::DisabledOpacityProperty(); } },
          { L"dragopacity"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::DragOpacityProperty(); } },
          { L"reorderhintoffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::ReorderHintOffsetProperty(); } },
          { L"gridviewitempresenterhorizontalcontentalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::GridViewItemPresenterHorizontalContentAlignmentProperty(); } },
          { L"gridviewitempresenterverticalcontentalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::GridViewItemPresenterVerticalContentAlignmentProperty(); } },
          { L"gridviewitempresenterpadding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::GridViewItemPresenterPaddingProperty(); } },
          { L"pointeroverbackgroundmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::PointerOverBackgroundMarginProperty(); } },
          { L"contentmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::GridViewItemPresenter::ContentMarginProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::InfoBarPanel::HorizontalOrientationPaddingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::InfoBarPanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"horizontalorientationpadding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::InfoBarPanel::HorizontalOrientationPaddingProperty(); } },
          { L"verticalorientationpadding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::InfoBarPanel::VerticalOrientationPaddingProperty(); } },
          { L"horizontalorientationmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::InfoBarPanel::HorizontalOrientationMarginProperty(); } },
          { L"verticalorientationmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::InfoBarPanel::VerticalOrientationMarginProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::JumpListItemBackgroundConverter::EnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::JumpListItemBackgroundConverter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::JumpListItemBackgroundConverter::EnabledProperty(); } },
          { L"disabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::JumpListItemBackgroundConverter::DisabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::JumpListItemForegroundConverter::EnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::JumpListItemForegroundConverter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::JumpListItemForegroundConverter::EnabledProperty(); } },
          { L"disabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::JumpListItemForegroundConverter::DisabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionCheckMarkVisualEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectioncheckmarkvisualenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionCheckMarkVisualEnabledProperty(); } },
          { L"checkhintbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckHintBrushProperty(); } },
          { L"checkselectingbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckSelectingBrushProperty(); } },
          { L"checkbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBrushProperty(); } },
          { L"dragbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::DragBackgroundProperty(); } },
          { L"dragforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::DragForegroundProperty(); } },
          { L"focusborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::FocusBorderBrushProperty(); } },
          { L"placeholderbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PlaceholderBackgroundProperty(); } },
          { L"pointeroverbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PointerOverBackgroundProperty(); } },
          { L"selectedbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedBackgroundProperty(); } },
          { L"selectedforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedForegroundProperty(); } },
          { L"selectedpointeroverbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedPointerOverBackgroundProperty(); } },
          { L"selectedpointeroverborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedPointerOverBorderBrushProperty(); } },
          { L"selectedborderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedBorderThicknessProperty(); } },
          { L"disabledopacity"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::DisabledOpacityProperty(); } },
          { L"dragopacity"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::DragOpacityProperty(); } },
          { L"reorderhintoffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ReorderHintOffsetProperty(); } },
          { L"listviewitempresenterhorizontalcontentalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ListViewItemPresenterHorizontalContentAlignmentProperty(); } },
          { L"listviewitempresenterverticalcontentalignment"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ListViewItemPresenterVerticalContentAlignmentProperty(); } },
          { L"listviewitempresenterpadding"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ListViewItemPresenterPaddingProperty(); } },
          { L"pointeroverbackgroundmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PointerOverBackgroundMarginProperty(); } },
          { L"contentmargin"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ContentMarginProperty(); } },
          { L"selectedpressedbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedPressedBackgroundProperty(); } },
          { L"pressedbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PressedBackgroundProperty(); } },
          { L"checkboxbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxBrushProperty(); } },
          { L"focussecondaryborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::FocusSecondaryBorderBrushProperty(); } },
          { L"checkmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckModeProperty(); } },
          { L"pointeroverforeground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PointerOverForegroundProperty(); } },
          { L"revealbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::RevealBackgroundProperty(); } },
          { L"revealborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::RevealBorderBrushProperty(); } },
          { L"revealborderthickness"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::RevealBorderThicknessProperty(); } },
          { L"revealbackgroundshowsabovecontent"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::RevealBackgroundShowsAboveContentProperty(); } },
          { L"selecteddisabledbackground"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedDisabledBackgroundProperty(); } },
          { L"checkpressedbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckPressedBrushProperty(); } },
          { L"checkdisabledbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckDisabledBrushProperty(); } },
          { L"checkboxpointeroverbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxPointerOverBrushProperty(); } },
          { L"checkboxpressedbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxPressedBrushProperty(); } },
          { L"checkboxdisabledbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxDisabledBrushProperty(); } },
          { L"checkboxselectedbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxSelectedBrushProperty(); } },
          { L"checkboxselectedpointeroverbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxSelectedPointerOverBrushProperty(); } },
          { L"checkboxselectedpressedbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxSelectedPressedBrushProperty(); } },
          { L"checkboxselecteddisabledbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxSelectedDisabledBrushProperty(); } },
          { L"checkboxborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxBorderBrushProperty(); } },
          { L"checkboxpointeroverborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxPointerOverBorderBrushProperty(); } },
          { L"checkboxpressedborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxPressedBorderBrushProperty(); } },
          { L"checkboxdisabledborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxDisabledBorderBrushProperty(); } },
          { L"checkboxcornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxCornerRadiusProperty(); } },
          { L"selectionindicatorcornerradius"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorCornerRadiusProperty(); } },
          { L"selectionindicatorvisualenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorVisualEnabledProperty(); } },
          { L"selectionindicatormode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorModeProperty(); } },
          { L"selectionindicatorbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorBrushProperty(); } },
          { L"selectionindicatorpointeroverbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorPointerOverBrushProperty(); } },
          { L"selectionindicatorpressedbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorPressedBrushProperty(); } },
          { L"selectionindicatordisabledbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorDisabledBrushProperty(); } },
          { L"selectedborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedBorderBrushProperty(); } },
          { L"selectedpressedborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedPressedBorderBrushProperty(); } },
          { L"selecteddisabledborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedDisabledBorderBrushProperty(); } },
          { L"selectedinnerborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedInnerBorderBrushProperty(); } },
          { L"pointeroverborderbrush"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PointerOverBorderBrushProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::LoopingSelector::ShouldLoopProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::LoopingSelector>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"shouldloop"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::LoopingSelector::ShouldLoopProperty(); } },
          { L"items"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::LoopingSelector::ItemsProperty(); } },
          { L"selectedindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::LoopingSelector::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::LoopingSelector::SelectedItemProperty(); } },
          { L"itemwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::LoopingSelector::ItemWidthProperty(); } },
          { L"itemheight"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::LoopingSelector::ItemHeightProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::LoopingSelector::ItemTemplateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::MonochromaticOverlayPresenter::SourceElementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::MonochromaticOverlayPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"sourceelement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::MonochromaticOverlayPresenter::SourceElementProperty(); } },
          { L"replacementcolor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::MonochromaticOverlayPresenter::ReplacementColorProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::NavigationViewItemPresenter::IconProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::NavigationViewItemPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"icon"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::NavigationViewItemPresenter::IconProperty(); } },
          { L"templatesettings"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::NavigationViewItemPresenter::TemplateSettingsProperty(); } },
          { L"infobadge"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::NavigationViewItemPresenter::InfoBadgeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::NavigationViewItemPresenterTemplateSettings::IconWidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::NavigationViewItemPresenterTemplateSettings>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iconwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::NavigationViewItemPresenterTemplateSettings::IconWidthProperty(); } },
          { L"smallericonwidth"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::NavigationViewItemPresenterTemplateSettings::SmallerIconWidthProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::PickerFlyoutBase::TitleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::PickerFlyoutBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"title"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::PickerFlyoutBase::TitleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::ChildProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"child"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::ChildProperty(); } },
          { L"isopen"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::IsOpenProperty(); } },
          { L"horizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::HorizontalOffsetProperty(); } },
          { L"verticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::VerticalOffsetProperty(); } },
          { L"childtransitions"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::ChildTransitionsProperty(); } },
          { L"islightdismissenabled"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::IsLightDismissEnabledProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::LightDismissOverlayModeProperty(); } },
          { L"shouldconstraintorootbounds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::ShouldConstrainToRootBoundsProperty(); } },
          { L"placementtarget"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::PlacementTargetProperty(); } },
          { L"desiredplacement"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::DesiredPlacementProperty(); } },
          { L"systembackdrop"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup::SystemBackdropProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBase::MinimumProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"minimum"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBase::MinimumProperty(); } },
          { L"maximum"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBase::MaximumProperty(); } },
          { L"smallchange"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBase::SmallChangeProperty(); } },
          { L"largechange"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBase::LargeChangeProperty(); } },
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBase::ValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::RepeatButton::DelayProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::RepeatButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"delay"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::RepeatButton::DelayProperty(); } },
          { L"interval"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::RepeatButton::IntervalProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollBar::OrientationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"orientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollBar::OrientationProperty(); } },
          { L"viewportsize"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollBar::ViewportSizeProperty(); } },
          { L"indicatormode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollBar::IndicatorModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::BackgroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"background"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::BackgroundProperty(); } },
          { L"content"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::ContentProperty(); } },
          { L"contentorientation"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::ContentOrientationProperty(); } },
          { L"horizontalscrollchainmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::HorizontalScrollChainModeProperty(); } },
          { L"verticalscrollchainmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::VerticalScrollChainModeProperty(); } },
          { L"horizontalscrollrailmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::HorizontalScrollRailModeProperty(); } },
          { L"verticalscrollrailmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::VerticalScrollRailModeProperty(); } },
          { L"horizontalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::HorizontalScrollModeProperty(); } },
          { L"verticalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::VerticalScrollModeProperty(); } },
          { L"computedhorizontalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::ComputedHorizontalScrollModeProperty(); } },
          { L"computedverticalscrollmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::ComputedVerticalScrollModeProperty(); } },
          { L"zoomchainmode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::ZoomChainModeProperty(); } },
          { L"zoommode"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::ZoomModeProperty(); } },
          { L"ignoredinputkinds"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::IgnoredInputKindsProperty(); } },
          { L"minzoomfactor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::MinZoomFactorProperty(); } },
          { L"maxzoomfactor"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::MaxZoomFactorProperty(); } },
          { L"horizontalanchorratio"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::HorizontalAnchorRatioProperty(); } },
          { L"verticalanchorratio"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollPresenter::VerticalAnchorRatioProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::Selector::SelectedIndexProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::Selector>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectedindex"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Selector::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Selector::SelectedItemProperty(); } },
          { L"selectedvalue"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Selector::SelectedValueProperty(); } },
          { L"selectedvaluepath"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Selector::SelectedValuePathProperty(); } },
          { L"issynchronizedwithcurrentitem"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Selector::IsSynchronizedWithCurrentItemProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::SelectorItem::IsSelectedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::SelectorItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isselected"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::SelectorItem::IsSelectedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::Thumb::IsDraggingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::Thumb>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdragging"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::Thumb::IsDraggingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::TickBar::FillProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::TickBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fill"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::TickBar::FillProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Controls::Primitives::ToggleButton::IsCheckedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Controls::Primitives::ToggleButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ischecked"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ToggleButton::IsCheckedProperty(); } },
          { L"isthreestate"s, []() { return winrt::Microsoft::UI::Xaml::Controls::Primitives::ToggleButton::IsThreeStateProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Data.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Data::CollectionViewSource::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Data::CollectionViewSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Microsoft::UI::Xaml::Data::CollectionViewSource::SourceProperty(); } },
          { L"view"s, []() { return winrt::Microsoft::UI::Xaml::Data::CollectionViewSource::ViewProperty(); } },
          { L"issourcegrouped"s, []() { return winrt::Microsoft::UI::Xaml::Data::CollectionViewSource::IsSourceGroupedProperty(); } },
          { L"itemspath"s, []() { return winrt::Microsoft::UI::Xaml::Data::CollectionViewSource::ItemsPathProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Documents.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Documents::Block::TextAlignmentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Documents::Block>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"textalignment"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Block::TextAlignmentProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Block::HorizontalTextAlignmentProperty(); } },
          { L"lineheight"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Block::LineHeightProperty(); } },
          { L"linestackingstrategy"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Block::LineStackingStrategyProperty(); } },
          { L"margin"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Block::MarginProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Documents::Glyphs::UnicodeStringProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Documents::Glyphs>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"unicodestring"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::UnicodeStringProperty(); } },
          { L"indices"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::IndicesProperty(); } },
          { L"fonturi"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::FontUriProperty(); } },
          { L"stylesimulations"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::StyleSimulationsProperty(); } },
          { L"fontrenderingemsize"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::FontRenderingEmSizeProperty(); } },
          { L"originx"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::OriginXProperty(); } },
          { L"originy"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::OriginYProperty(); } },
          { L"fill"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::FillProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::IsColorFontEnabledProperty(); } },
          { L"colorfontpaletteindex"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Glyphs::ColorFontPaletteIndexProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Documents::Hyperlink::NavigateUriProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Documents::Hyperlink>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"navigateuri"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::NavigateUriProperty(); } },
          { L"underlinestyle"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::UnderlineStyleProperty(); } },
          { L"xyfocusleft"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::XYFocusLeftProperty(); } },
          { L"xyfocusright"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::XYFocusRightProperty(); } },
          { L"xyfocusup"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::XYFocusUpProperty(); } },
          { L"xyfocusdown"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::XYFocusDownProperty(); } },
          { L"elementsoundmode"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::ElementSoundModeProperty(); } },
          { L"focusstate"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::FocusStateProperty(); } },
          { L"xyfocusupnavigationstrategy"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::XYFocusUpNavigationStrategyProperty(); } },
          { L"xyfocusdownnavigationstrategy"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::XYFocusDownNavigationStrategyProperty(); } },
          { L"xyfocusleftnavigationstrategy"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::XYFocusLeftNavigationStrategyProperty(); } },
          { L"xyfocusrightnavigationstrategy"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::XYFocusRightNavigationStrategyProperty(); } },
          { L"istabstop"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::IsTabStopProperty(); } },
          { L"tabindex"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Hyperlink::TabIndexProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Documents::Paragraph::TextIndentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Documents::Paragraph>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"textindent"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Paragraph::TextIndentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Documents::Run::FlowDirectionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Documents::Run>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"flowdirection"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Run::FlowDirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Documents::TextElement::FontSizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Documents::TextElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fontsize"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::FontStretchProperty(); } },
          { L"characterspacing"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::CharacterSpacingProperty(); } },
          { L"foreground"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::ForegroundProperty(); } },
          { L"language"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::LanguageProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::IsTextScaleFactorEnabledProperty(); } },
          { L"textdecorations"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::TextDecorationsProperty(); } },
          { L"allowfocusoninteraction"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::AllowFocusOnInteractionProperty(); } },
          { L"accesskey"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::AccessKeyProperty(); } },
          { L"exitdisplaymodeonaccesskeyinvoked"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::ExitDisplayModeOnAccessKeyInvokedProperty(); } },
          { L"isaccesskeyscope"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::IsAccessKeyScopeProperty(); } },
          { L"accesskeyscopeowner"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::AccessKeyScopeOwnerProperty(); } },
          { L"keytipplacementmode"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::KeyTipPlacementModeProperty(); } },
          { L"keytiphorizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::KeyTipHorizontalOffsetProperty(); } },
          { L"keytipverticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextElement::KeyTipVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Documents::TextHighlighter::ForegroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Documents::TextHighlighter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"foreground"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextHighlighter::ForegroundProperty(); } },
          { L"background"s, []() { return winrt::Microsoft::UI::Xaml::Documents::TextHighlighter::BackgroundProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Documents::Typography::AnnotationAlternatesProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Documents::Typography>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"annotationalternates"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::AnnotationAlternatesProperty(); } },
          { L"eastasianexpertforms"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::EastAsianExpertFormsProperty(); } },
          { L"eastasianlanguage"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::EastAsianLanguageProperty(); } },
          { L"eastasianwidths"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::EastAsianWidthsProperty(); } },
          { L"standardligatures"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StandardLigaturesProperty(); } },
          { L"contextualligatures"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::ContextualLigaturesProperty(); } },
          { L"discretionaryligatures"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::DiscretionaryLigaturesProperty(); } },
          { L"historicalligatures"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::HistoricalLigaturesProperty(); } },
          { L"standardswashes"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StandardSwashesProperty(); } },
          { L"contextualswashes"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::ContextualSwashesProperty(); } },
          { L"contextualalternates"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::ContextualAlternatesProperty(); } },
          { L"stylisticalternates"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticAlternatesProperty(); } },
          { L"stylisticset1"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet1Property(); } },
          { L"stylisticset2"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet2Property(); } },
          { L"stylisticset3"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet3Property(); } },
          { L"stylisticset4"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet4Property(); } },
          { L"stylisticset5"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet5Property(); } },
          { L"stylisticset6"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet6Property(); } },
          { L"stylisticset7"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet7Property(); } },
          { L"stylisticset8"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet8Property(); } },
          { L"stylisticset9"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet9Property(); } },
          { L"stylisticset10"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet10Property(); } },
          { L"stylisticset11"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet11Property(); } },
          { L"stylisticset12"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet12Property(); } },
          { L"stylisticset13"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet13Property(); } },
          { L"stylisticset14"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet14Property(); } },
          { L"stylisticset15"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet15Property(); } },
          { L"stylisticset16"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet16Property(); } },
          { L"stylisticset17"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet17Property(); } },
          { L"stylisticset18"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet18Property(); } },
          { L"stylisticset19"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet19Property(); } },
          { L"stylisticset20"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::StylisticSet20Property(); } },
          { L"capitals"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::CapitalsProperty(); } },
          { L"capitalspacing"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::CapitalSpacingProperty(); } },
          { L"kerning"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::KerningProperty(); } },
          { L"casesensitiveforms"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::CaseSensitiveFormsProperty(); } },
          { L"historicalforms"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::HistoricalFormsProperty(); } },
          { L"fraction"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::FractionProperty(); } },
          { L"numeralstyle"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::NumeralStyleProperty(); } },
          { L"numeralalignment"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::NumeralAlignmentProperty(); } },
          { L"slashedzero"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::SlashedZeroProperty(); } },
          { L"mathematicalgreek"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::MathematicalGreekProperty(); } },
          { L"variants"s, []() { return winrt::Microsoft::UI::Xaml::Documents::Typography::VariantsProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Input.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator::KeyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"key"s, []() { return winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator::KeyProperty(); } },
          { L"modifiers"s, []() { return winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator::ModifiersProperty(); } },
          { L"isenabled"s, []() { return winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator::IsEnabledProperty(); } },
          { L"scopeowner"s, []() { return winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator::ScopeOwnerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Input::StandardUICommand::KindProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Input::StandardUICommand>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"kind"s, []() { return winrt::Microsoft::UI::Xaml::Input::StandardUICommand::KindProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Input::XamlUICommand::LabelProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Input::XamlUICommand>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"label"s, []() { return winrt::Microsoft::UI::Xaml::Input::XamlUICommand::LabelProperty(); } },
          { L"iconsource"s, []() { return winrt::Microsoft::UI::Xaml::Input::XamlUICommand::IconSourceProperty(); } },
          { L"keyboardaccelerators"s, []() { return winrt::Microsoft::UI::Xaml::Input::XamlUICommand::KeyboardAcceleratorsProperty(); } },
          { L"accesskey"s, []() { return winrt::Microsoft::UI::Xaml::Input::XamlUICommand::AccessKeyProperty(); } },
          { L"description"s, []() { return winrt::Microsoft::UI::Xaml::Input::XamlUICommand::DescriptionProperty(); } },
          { L"command"s, []() { return winrt::Microsoft::UI::Xaml::Input::XamlUICommand::CommandProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Markup.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Markup::XamlBindingHelper::DataTemplateComponentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Markup::XamlBindingHelper>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"datatemplatecomponent"s, []() { return winrt::Microsoft::UI::Xaml::Markup::XamlBindingHelper::DataTemplateComponentProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Media.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::AcrylicBrush::TintColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::AcrylicBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"tintcolor"s, []() { return winrt::Microsoft::UI::Xaml::Media::AcrylicBrush::TintColorProperty(); } },
          { L"tintopacity"s, []() { return winrt::Microsoft::UI::Xaml::Media::AcrylicBrush::TintOpacityProperty(); } },
          { L"tinttransitionduration"s, []() { return winrt::Microsoft::UI::Xaml::Media::AcrylicBrush::TintTransitionDurationProperty(); } },
          { L"alwaysusefallback"s, []() { return winrt::Microsoft::UI::Xaml::Media::AcrylicBrush::AlwaysUseFallbackProperty(); } },
          { L"tintluminosityopacity"s, []() { return winrt::Microsoft::UI::Xaml::Media::AcrylicBrush::TintLuminosityOpacityProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::ArcSegment::PointProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::ArcSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"point"s, []() { return winrt::Microsoft::UI::Xaml::Media::ArcSegment::PointProperty(); } },
          { L"size"s, []() { return winrt::Microsoft::UI::Xaml::Media::ArcSegment::SizeProperty(); } },
          { L"rotationangle"s, []() { return winrt::Microsoft::UI::Xaml::Media::ArcSegment::RotationAngleProperty(); } },
          { L"islargearc"s, []() { return winrt::Microsoft::UI::Xaml::Media::ArcSegment::IsLargeArcProperty(); } },
          { L"sweepdirection"s, []() { return winrt::Microsoft::UI::Xaml::Media::ArcSegment::SweepDirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::BezierSegment::Point1Property())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::BezierSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"point1"s, []() { return winrt::Microsoft::UI::Xaml::Media::BezierSegment::Point1Property(); } },
          { L"point2"s, []() { return winrt::Microsoft::UI::Xaml::Media::BezierSegment::Point2Property(); } },
          { L"point3"s, []() { return winrt::Microsoft::UI::Xaml::Media::BezierSegment::Point3Property(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Brush::OpacityProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Brush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"opacity"s, []() { return winrt::Microsoft::UI::Xaml::Media::Brush::OpacityProperty(); } },
          { L"transform"s, []() { return winrt::Microsoft::UI::Xaml::Media::Brush::TransformProperty(); } },
          { L"relativetransform"s, []() { return winrt::Microsoft::UI::Xaml::Media::Brush::RelativeTransformProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::CompositeTransform::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::CompositeTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Microsoft::UI::Xaml::Media::CompositeTransform::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Microsoft::UI::Xaml::Media::CompositeTransform::CenterYProperty(); } },
          { L"scalex"s, []() { return winrt::Microsoft::UI::Xaml::Media::CompositeTransform::ScaleXProperty(); } },
          { L"scaley"s, []() { return winrt::Microsoft::UI::Xaml::Media::CompositeTransform::ScaleYProperty(); } },
          { L"skewx"s, []() { return winrt::Microsoft::UI::Xaml::Media::CompositeTransform::SkewXProperty(); } },
          { L"skewy"s, []() { return winrt::Microsoft::UI::Xaml::Media::CompositeTransform::SkewYProperty(); } },
          { L"rotation"s, []() { return winrt::Microsoft::UI::Xaml::Media::CompositeTransform::RotationProperty(); } },
          { L"translatex"s, []() { return winrt::Microsoft::UI::Xaml::Media::CompositeTransform::TranslateXProperty(); } },
          { L"translatey"s, []() { return winrt::Microsoft::UI::Xaml::Media::CompositeTransform::TranslateYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::EllipseGeometry::CenterProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::EllipseGeometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"center"s, []() { return winrt::Microsoft::UI::Xaml::Media::EllipseGeometry::CenterProperty(); } },
          { L"radiusx"s, []() { return winrt::Microsoft::UI::Xaml::Media::EllipseGeometry::RadiusXProperty(); } },
          { L"radiusy"s, []() { return winrt::Microsoft::UI::Xaml::Media::EllipseGeometry::RadiusYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Geometry::TransformProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Geometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"transform"s, []() { return winrt::Microsoft::UI::Xaml::Media::Geometry::TransformProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::GeometryGroup::FillRuleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::GeometryGroup>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fillrule"s, []() { return winrt::Microsoft::UI::Xaml::Media::GeometryGroup::FillRuleProperty(); } },
          { L"children"s, []() { return winrt::Microsoft::UI::Xaml::Media::GeometryGroup::ChildrenProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::GradientBrush::SpreadMethodProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::GradientBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"spreadmethod"s, []() { return winrt::Microsoft::UI::Xaml::Media::GradientBrush::SpreadMethodProperty(); } },
          { L"mappingmode"s, []() { return winrt::Microsoft::UI::Xaml::Media::GradientBrush::MappingModeProperty(); } },
          { L"colorinterpolationmode"s, []() { return winrt::Microsoft::UI::Xaml::Media::GradientBrush::ColorInterpolationModeProperty(); } },
          { L"gradientstops"s, []() { return winrt::Microsoft::UI::Xaml::Media::GradientBrush::GradientStopsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::GradientStop::ColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::GradientStop>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"color"s, []() { return winrt::Microsoft::UI::Xaml::Media::GradientStop::ColorProperty(); } },
          { L"offset"s, []() { return winrt::Microsoft::UI::Xaml::Media::GradientStop::OffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::ImageBrush::ImageSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::ImageBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"imagesource"s, []() { return winrt::Microsoft::UI::Xaml::Media::ImageBrush::ImageSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::LineGeometry::StartPointProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::LineGeometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"startpoint"s, []() { return winrt::Microsoft::UI::Xaml::Media::LineGeometry::StartPointProperty(); } },
          { L"endpoint"s, []() { return winrt::Microsoft::UI::Xaml::Media::LineGeometry::EndPointProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::LineSegment::PointProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::LineSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"point"s, []() { return winrt::Microsoft::UI::Xaml::Media::LineSegment::PointProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::LinearGradientBrush::StartPointProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::LinearGradientBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"startpoint"s, []() { return winrt::Microsoft::UI::Xaml::Media::LinearGradientBrush::StartPointProperty(); } },
          { L"endpoint"s, []() { return winrt::Microsoft::UI::Xaml::Media::LinearGradientBrush::EndPointProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Matrix3DProjection::ProjectionMatrixProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Matrix3DProjection>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"projectionmatrix"s, []() { return winrt::Microsoft::UI::Xaml::Media::Matrix3DProjection::ProjectionMatrixProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::MatrixTransform::MatrixProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::MatrixTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"matrix"s, []() { return winrt::Microsoft::UI::Xaml::Media::MatrixTransform::MatrixProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::MicaBackdrop::KindProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::MicaBackdrop>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"kind"s, []() { return winrt::Microsoft::UI::Xaml::Media::MicaBackdrop::KindProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::PathFigure::SegmentsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::PathFigure>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"segments"s, []() { return winrt::Microsoft::UI::Xaml::Media::PathFigure::SegmentsProperty(); } },
          { L"startpoint"s, []() { return winrt::Microsoft::UI::Xaml::Media::PathFigure::StartPointProperty(); } },
          { L"isclosed"s, []() { return winrt::Microsoft::UI::Xaml::Media::PathFigure::IsClosedProperty(); } },
          { L"isfilled"s, []() { return winrt::Microsoft::UI::Xaml::Media::PathFigure::IsFilledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::PathGeometry::FillRuleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::PathGeometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fillrule"s, []() { return winrt::Microsoft::UI::Xaml::Media::PathGeometry::FillRuleProperty(); } },
          { L"figures"s, []() { return winrt::Microsoft::UI::Xaml::Media::PathGeometry::FiguresProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::PlaneProjection::LocalOffsetXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::PlaneProjection>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"localoffsetx"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::LocalOffsetXProperty(); } },
          { L"localoffsety"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::LocalOffsetYProperty(); } },
          { L"localoffsetz"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::LocalOffsetZProperty(); } },
          { L"rotationx"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::RotationXProperty(); } },
          { L"rotationy"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::RotationYProperty(); } },
          { L"rotationz"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::RotationZProperty(); } },
          { L"centerofrotationx"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::CenterOfRotationXProperty(); } },
          { L"centerofrotationy"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::CenterOfRotationYProperty(); } },
          { L"centerofrotationz"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::CenterOfRotationZProperty(); } },
          { L"globaloffsetx"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::GlobalOffsetXProperty(); } },
          { L"globaloffsety"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::GlobalOffsetYProperty(); } },
          { L"globaloffsetz"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::GlobalOffsetZProperty(); } },
          { L"projectionmatrix"s, []() { return winrt::Microsoft::UI::Xaml::Media::PlaneProjection::ProjectionMatrixProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::PolyBezierSegment::PointsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::PolyBezierSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"points"s, []() { return winrt::Microsoft::UI::Xaml::Media::PolyBezierSegment::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::PolyLineSegment::PointsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::PolyLineSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"points"s, []() { return winrt::Microsoft::UI::Xaml::Media::PolyLineSegment::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::PolyQuadraticBezierSegment::PointsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::PolyQuadraticBezierSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"points"s, []() { return winrt::Microsoft::UI::Xaml::Media::PolyQuadraticBezierSegment::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::QuadraticBezierSegment::Point1Property())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::QuadraticBezierSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"point1"s, []() { return winrt::Microsoft::UI::Xaml::Media::QuadraticBezierSegment::Point1Property(); } },
          { L"point2"s, []() { return winrt::Microsoft::UI::Xaml::Media::QuadraticBezierSegment::Point2Property(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::RadialGradientBrush::CenterProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::RadialGradientBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"center"s, []() { return winrt::Microsoft::UI::Xaml::Media::RadialGradientBrush::CenterProperty(); } },
          { L"radiusx"s, []() { return winrt::Microsoft::UI::Xaml::Media::RadialGradientBrush::RadiusXProperty(); } },
          { L"radiusy"s, []() { return winrt::Microsoft::UI::Xaml::Media::RadialGradientBrush::RadiusYProperty(); } },
          { L"gradientorigin"s, []() { return winrt::Microsoft::UI::Xaml::Media::RadialGradientBrush::GradientOriginProperty(); } },
          { L"interpolationspace"s, []() { return winrt::Microsoft::UI::Xaml::Media::RadialGradientBrush::InterpolationSpaceProperty(); } },
          { L"mappingmode"s, []() { return winrt::Microsoft::UI::Xaml::Media::RadialGradientBrush::MappingModeProperty(); } },
          { L"spreadmethod"s, []() { return winrt::Microsoft::UI::Xaml::Media::RadialGradientBrush::SpreadMethodProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::RectangleGeometry::RectProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::RectangleGeometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"rect"s, []() { return winrt::Microsoft::UI::Xaml::Media::RectangleGeometry::RectProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::RotateTransform::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::RotateTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Microsoft::UI::Xaml::Media::RotateTransform::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Microsoft::UI::Xaml::Media::RotateTransform::CenterYProperty(); } },
          { L"angle"s, []() { return winrt::Microsoft::UI::Xaml::Media::RotateTransform::AngleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::ScaleTransform::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::ScaleTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Microsoft::UI::Xaml::Media::ScaleTransform::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Microsoft::UI::Xaml::Media::ScaleTransform::CenterYProperty(); } },
          { L"scalex"s, []() { return winrt::Microsoft::UI::Xaml::Media::ScaleTransform::ScaleXProperty(); } },
          { L"scaley"s, []() { return winrt::Microsoft::UI::Xaml::Media::ScaleTransform::ScaleYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::SkewTransform::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::SkewTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Microsoft::UI::Xaml::Media::SkewTransform::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Microsoft::UI::Xaml::Media::SkewTransform::CenterYProperty(); } },
          { L"anglex"s, []() { return winrt::Microsoft::UI::Xaml::Media::SkewTransform::AngleXProperty(); } },
          { L"angley"s, []() { return winrt::Microsoft::UI::Xaml::Media::SkewTransform::AngleYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush::ColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::SolidColorBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"color"s, []() { return winrt::Microsoft::UI::Xaml::Media::SolidColorBrush::ColorProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::TileBrush::AlignmentXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::TileBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"alignmentx"s, []() { return winrt::Microsoft::UI::Xaml::Media::TileBrush::AlignmentXProperty(); } },
          { L"alignmenty"s, []() { return winrt::Microsoft::UI::Xaml::Media::TileBrush::AlignmentYProperty(); } },
          { L"stretch"s, []() { return winrt::Microsoft::UI::Xaml::Media::TileBrush::StretchProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::TransformGroup::ChildrenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::TransformGroup>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"children"s, []() { return winrt::Microsoft::UI::Xaml::Media::TransformGroup::ChildrenProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::TranslateTransform::XProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::TranslateTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"x"s, []() { return winrt::Microsoft::UI::Xaml::Media::TranslateTransform::XProperty(); } },
          { L"y"s, []() { return winrt::Microsoft::UI::Xaml::Media::TranslateTransform::YProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::XamlCompositionBrushBase::FallbackColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::XamlCompositionBrushBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fallbackcolor"s, []() { return winrt::Microsoft::UI::Xaml::Media::XamlCompositionBrushBase::FallbackColorProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Media.Animation.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::BackEase::AmplitudeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::BackEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"amplitude"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::BackEase::AmplitudeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::BeginStoryboard::StoryboardProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::BeginStoryboard>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"storyboard"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::BeginStoryboard::StoryboardProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::BounceEase::BouncesProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::BounceEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"bounces"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::BounceEase::BouncesProperty(); } },
          { L"bounciness"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::BounceEase::BouncinessProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimation::FromProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"from"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimation::FromProperty(); } },
          { L"to"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimation::ToProperty(); } },
          { L"by"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimation::ByProperty(); } },
          { L"easingfunction"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimation::EasingFunctionProperty(); } },
          { L"enabledependentanimation"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimation::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimationUsingKeyFrames::EnableDependentAnimationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimationUsingKeyFrames>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabledependentanimation"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ColorAnimationUsingKeyFrames::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::ColorKeyFrame::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::ColorKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ColorKeyFrame::ValueProperty(); } },
          { L"keytime"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ColorKeyFrame::KeyTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::CommonNavigationTransitionInfo::IsStaggeringEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::CommonNavigationTransitionInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isstaggeringenabled"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::CommonNavigationTransitionInfo::IsStaggeringEnabledProperty(); } },
          { L"isstaggerelement"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::CommonNavigationTransitionInfo::IsStaggerElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::ContentThemeTransition::HorizontalOffsetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::ContentThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"horizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ContentThemeTransition::HorizontalOffsetProperty(); } },
          { L"verticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ContentThemeTransition::VerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::ExitElementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"exitelement"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::ExitElementProperty(); } },
          { L"isentranceelement"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::IsEntranceElementProperty(); } },
          { L"isexitelement"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::IsExitElementProperty(); } },
          { L"exitelementcontainer"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::ExitElementContainerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation::FromProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"from"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation::FromProperty(); } },
          { L"to"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation::ToProperty(); } },
          { L"by"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation::ByProperty(); } },
          { L"easingfunction"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation::EasingFunctionProperty(); } },
          { L"enabledependentanimation"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimationUsingKeyFrames::EnableDependentAnimationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimationUsingKeyFrames>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabledependentanimation"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimationUsingKeyFrames::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::DoubleKeyFrame::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::DoubleKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DoubleKeyFrame::ValueProperty(); } },
          { L"keytime"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DoubleKeyFrame::KeyTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::DragItemThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::DragItemThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DragItemThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::DragOverThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::DragOverThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DragOverThemeAnimation::TargetNameProperty(); } },
          { L"tooffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DragOverThemeAnimation::ToOffsetProperty(); } },
          { L"direction"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DragOverThemeAnimation::DirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::DrillInThemeAnimation::EntranceTargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::DrillInThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"entrancetargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DrillInThemeAnimation::EntranceTargetNameProperty(); } },
          { L"entrancetarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DrillInThemeAnimation::EntranceTargetProperty(); } },
          { L"exittargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DrillInThemeAnimation::ExitTargetNameProperty(); } },
          { L"exittarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DrillInThemeAnimation::ExitTargetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::DrillOutThemeAnimation::EntranceTargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::DrillOutThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"entrancetargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DrillOutThemeAnimation::EntranceTargetNameProperty(); } },
          { L"entrancetarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DrillOutThemeAnimation::EntranceTargetProperty(); } },
          { L"exittargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DrillOutThemeAnimation::ExitTargetNameProperty(); } },
          { L"exittarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DrillOutThemeAnimation::ExitTargetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::DropTargetItemThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::DropTargetItemThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::DropTargetItemThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::EasingColorKeyFrame::EasingFunctionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::EasingColorKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"easingfunction"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::EasingColorKeyFrame::EasingFunctionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::EasingDoubleKeyFrame::EasingFunctionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::EasingDoubleKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"easingfunction"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::EasingDoubleKeyFrame::EasingFunctionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::EasingFunctionBase::EasingModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::EasingFunctionBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"easingmode"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::EasingFunctionBase::EasingModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::EasingPointKeyFrame::EasingFunctionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::EasingPointKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"easingfunction"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::EasingPointKeyFrame::EasingFunctionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::EdgeUIThemeTransition::EdgeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::EdgeUIThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"edge"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::EdgeUIThemeTransition::EdgeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::ElasticEase::OscillationsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::ElasticEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"oscillations"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ElasticEase::OscillationsProperty(); } },
          { L"springiness"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ElasticEase::SpringinessProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::EntranceNavigationTransitionInfo::IsTargetElementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::EntranceNavigationTransitionInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"istargetelement"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::EntranceNavigationTransitionInfo::IsTargetElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::EntranceThemeTransition::FromHorizontalOffsetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::EntranceThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fromhorizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::EntranceThemeTransition::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::EntranceThemeTransition::FromVerticalOffsetProperty(); } },
          { L"isstaggeringenabled"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::EntranceThemeTransition::IsStaggeringEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::ExponentialEase::ExponentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::ExponentialEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"exponent"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ExponentialEase::ExponentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::FadeInThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::FadeInThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::FadeInThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::FadeOutThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::FadeOutThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::FadeOutThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::NavigationThemeTransition::DefaultNavigationTransitionInfoProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::NavigationThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"defaultnavigationtransitioninfo"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::NavigationThemeTransition::DefaultNavigationTransitionInfoProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::ObjectAnimationUsingKeyFrames::EnableDependentAnimationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::ObjectAnimationUsingKeyFrames>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabledependentanimation"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ObjectAnimationUsingKeyFrames::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::ObjectKeyFrame::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::ObjectKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ObjectKeyFrame::ValueProperty(); } },
          { L"keytime"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::ObjectKeyFrame::KeyTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PaneThemeTransition::EdgeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PaneThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"edge"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PaneThemeTransition::EdgeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimation::FromProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"from"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimation::FromProperty(); } },
          { L"to"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimation::ToProperty(); } },
          { L"by"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimation::ByProperty(); } },
          { L"easingfunction"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimation::EasingFunctionProperty(); } },
          { L"enabledependentanimation"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimation::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimationUsingKeyFrames::EnableDependentAnimationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimationUsingKeyFrames>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabledependentanimation"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointAnimationUsingKeyFrames::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PointKeyFrame::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PointKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"value"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointKeyFrame::ValueProperty(); } },
          { L"keytime"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointKeyFrame::KeyTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PointerDownThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PointerDownThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointerDownThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PointerUpThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PointerUpThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PointerUpThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PopInThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PopInThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PopInThemeAnimation::TargetNameProperty(); } },
          { L"fromhorizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PopInThemeAnimation::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PopInThemeAnimation::FromVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PopOutThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PopOutThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PopOutThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PopupThemeTransition::FromHorizontalOffsetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PopupThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fromhorizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PopupThemeTransition::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PopupThemeTransition::FromVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::PowerEase::PowerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::PowerEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"power"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::PowerEase::PowerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::RepositionThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::RepositionThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::RepositionThemeAnimation::TargetNameProperty(); } },
          { L"fromhorizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::RepositionThemeAnimation::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::RepositionThemeAnimation::FromVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::RepositionThemeTransition::IsStaggeringEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::RepositionThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isstaggeringenabled"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::RepositionThemeTransition::IsStaggeringEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::SlideNavigationTransitionInfo::EffectProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::SlideNavigationTransitionInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"effect"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SlideNavigationTransitionInfo::EffectProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::SplineColorKeyFrame::KeySplineProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::SplineColorKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"keyspline"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplineColorKeyFrame::KeySplineProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::SplineDoubleKeyFrame::KeySplineProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::SplineDoubleKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"keyspline"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplineDoubleKeyFrame::KeySplineProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::SplinePointKeyFrame::KeySplineProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::SplinePointKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"keyspline"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplinePointKeyFrame::KeySplineProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OpenedTargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"openedtargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OpenedTargetNameProperty(); } },
          { L"openedtarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OpenedTargetProperty(); } },
          { L"closedtargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ClosedTargetNameProperty(); } },
          { L"closedtarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ClosedTargetProperty(); } },
          { L"contenttargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ContentTargetNameProperty(); } },
          { L"contenttarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ContentTargetProperty(); } },
          { L"openedlength"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OpenedLengthProperty(); } },
          { L"closedlength"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ClosedLengthProperty(); } },
          { L"offsetfromcenter"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OffsetFromCenterProperty(); } },
          { L"contenttranslationdirection"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ContentTranslationDirectionProperty(); } },
          { L"contenttranslationoffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ContentTranslationOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OpenedTargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"openedtargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OpenedTargetNameProperty(); } },
          { L"openedtarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OpenedTargetProperty(); } },
          { L"closedtargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ClosedTargetNameProperty(); } },
          { L"closedtarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ClosedTargetProperty(); } },
          { L"contenttargetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ContentTargetNameProperty(); } },
          { L"contenttarget"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ContentTargetProperty(); } },
          { L"openedlength"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OpenedLengthProperty(); } },
          { L"closedlength"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ClosedLengthProperty(); } },
          { L"offsetfromcenter"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OffsetFromCenterProperty(); } },
          { L"contenttranslationdirection"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ContentTranslationDirectionProperty(); } },
          { L"contenttranslationoffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ContentTranslationOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::TargetPropertyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetproperty"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::TargetPropertyProperty(); } },
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::SwipeBackThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::SwipeBackThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SwipeBackThemeAnimation::TargetNameProperty(); } },
          { L"fromhorizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SwipeBackThemeAnimation::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SwipeBackThemeAnimation::FromVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::SwipeHintThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::SwipeHintThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SwipeHintThemeAnimation::TargetNameProperty(); } },
          { L"tohorizontaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SwipeHintThemeAnimation::ToHorizontalOffsetProperty(); } },
          { L"toverticaloffset"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::SwipeHintThemeAnimation::ToVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Animation::Timeline::AutoReverseProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Animation::Timeline>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"autoreverse"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::Timeline::AutoReverseProperty(); } },
          { L"begintime"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::Timeline::BeginTimeProperty(); } },
          { L"duration"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::Timeline::DurationProperty(); } },
          { L"speedratio"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::Timeline::SpeedRatioProperty(); } },
          { L"fillbehavior"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::Timeline::FillBehaviorProperty(); } },
          { L"repeatbehavior"s, []() { return winrt::Microsoft::UI::Xaml::Media::Animation::Timeline::RepeatBehaviorProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Media.Imaging.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage::CreateOptionsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"createoptions"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage::CreateOptionsProperty(); } },
          { L"urisource"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage::UriSourceProperty(); } },
          { L"decodepixelwidth"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage::DecodePixelWidthProperty(); } },
          { L"decodepixelheight"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage::DecodePixelHeightProperty(); } },
          { L"decodepixeltype"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage::DecodePixelTypeProperty(); } },
          { L"isanimatedbitmap"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage::IsAnimatedBitmapProperty(); } },
          { L"isplaying"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage::IsPlayingProperty(); } },
          { L"autoplay"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage::AutoPlayProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapSource::PixelWidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"pixelwidth"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapSource::PixelWidthProperty(); } },
          { L"pixelheight"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapSource::PixelHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Imaging::RenderTargetBitmap::PixelWidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Imaging::RenderTargetBitmap>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"pixelwidth"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::RenderTargetBitmap::PixelWidthProperty(); } },
          { L"pixelheight"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::RenderTargetBitmap::PixelHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource::UriSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"urisource"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource::UriSourceProperty(); } },
          { L"rasterizepixelwidth"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource::RasterizePixelWidthProperty(); } },
          { L"rasterizepixelheight"s, []() { return winrt::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource::RasterizePixelHeightProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Media.Media3D.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::CenterYProperty(); } },
          { L"centerz"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::CenterZProperty(); } },
          { L"rotationx"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::RotationXProperty(); } },
          { L"rotationy"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::RotationYProperty(); } },
          { L"rotationz"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::RotationZProperty(); } },
          { L"scalex"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::ScaleXProperty(); } },
          { L"scaley"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::ScaleYProperty(); } },
          { L"scalez"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::ScaleZProperty(); } },
          { L"translatex"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::TranslateXProperty(); } },
          { L"translatey"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::TranslateYProperty(); } },
          { L"translatez"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::CompositeTransform3D::TranslateZProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Media::Media3D::PerspectiveTransform3D::DepthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Media::Media3D::PerspectiveTransform3D>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"depth"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::PerspectiveTransform3D::DepthProperty(); } },
          { L"offsetx"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::PerspectiveTransform3D::OffsetXProperty(); } },
          { L"offsety"s, []() { return winrt::Microsoft::UI::Xaml::Media::Media3D::PerspectiveTransform3D::OffsetYProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Navigation.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Navigation::PageStackEntry::SourcePageTypeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Navigation::PageStackEntry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"sourcepagetype"s, []() { return winrt::Microsoft::UI::Xaml::Navigation::PageStackEntry::SourcePageTypeProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Printing.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Printing::PrintDocument::DocumentSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Printing::PrintDocument>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"documentsource"s, []() { return winrt::Microsoft::UI::Xaml::Printing::PrintDocument::DocumentSourceProperty(); } },
        }
      }
    ));
  }
  // Microsoft.UI.Xaml.Shapes.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Shapes::Line::X1Property())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Shapes::Line>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"x1"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Line::X1Property(); } },
          { L"y1"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Line::Y1Property(); } },
          { L"x2"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Line::X2Property(); } },
          { L"y2"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Line::Y2Property(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Shapes::Path::DataProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Shapes::Path>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"data"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Path::DataProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Shapes::Polygon::FillRuleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Shapes::Polygon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fillrule"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Polygon::FillRuleProperty(); } },
          { L"points"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Polygon::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Shapes::Polyline::FillRuleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Shapes::Polyline>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fillrule"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Polyline::FillRuleProperty(); } },
          { L"points"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Polyline::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Shapes::Rectangle::RadiusXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Shapes::Rectangle>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"radiusx"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Rectangle::RadiusXProperty(); } },
          { L"radiusy"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Rectangle::RadiusYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Microsoft::UI::Xaml::Shapes::Shape::FillProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Microsoft::UI::Xaml::Shapes::Shape>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fill"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::FillProperty(); } },
          { L"stroke"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StrokeProperty(); } },
          { L"strokemiterlimit"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StrokeMiterLimitProperty(); } },
          { L"strokethickness"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StrokeThicknessProperty(); } },
          { L"strokestartlinecap"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StrokeStartLineCapProperty(); } },
          { L"strokeendlinecap"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StrokeEndLineCapProperty(); } },
          { L"strokelinejoin"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StrokeLineJoinProperty(); } },
          { L"strokedashoffset"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StrokeDashOffsetProperty(); } },
          { L"strokedashcap"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StrokeDashCapProperty(); } },
          { L"strokedasharray"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StrokeDashArrayProperty(); } },
          { L"stretch"s, []() { return winrt::Microsoft::UI::Xaml::Shapes::Shape::StretchProperty(); } },
        }
      }
    ));
  }
  // MiniDumpExplorer.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::BoolToObjectConverter::TrueValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::BoolToObjectConverter>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"truevalue"s, []() { return winrt::MiniDumpExplorer::BoolToObjectConverter::TrueValueProperty(); } },
          { L"falsevalue"s, []() { return winrt::MiniDumpExplorer::BoolToObjectConverter::FalseValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::ControlSizeTrigger::CanTriggerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::ControlSizeTrigger>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"cantrigger"s, []() { return winrt::MiniDumpExplorer::ControlSizeTrigger::CanTriggerProperty(); } },
          { L"maxwidth"s, []() { return winrt::MiniDumpExplorer::ControlSizeTrigger::MaxWidthProperty(); } },
          { L"minwidth"s, []() { return winrt::MiniDumpExplorer::ControlSizeTrigger::MinWidthProperty(); } },
          { L"maxheight"s, []() { return winrt::MiniDumpExplorer::ControlSizeTrigger::MaxHeightProperty(); } },
          { L"minheight"s, []() { return winrt::MiniDumpExplorer::ControlSizeTrigger::MinHeightProperty(); } },
          { L"targetelement"s, []() { return winrt::MiniDumpExplorer::ControlSizeTrigger::TargetElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGrid::AlternatingRowBackgroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGrid>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"alternatingrowbackground"s, []() { return winrt::MiniDumpExplorer::DataGrid::AlternatingRowBackgroundProperty(); } },
          { L"alternatingrowforeground"s, []() { return winrt::MiniDumpExplorer::DataGrid::AlternatingRowForegroundProperty(); } },
          { L"arerowdetailsfrozen"s, []() { return winrt::MiniDumpExplorer::DataGrid::AreRowDetailsFrozenProperty(); } },
          { L"arerowgroupheadersfrozen"s, []() { return winrt::MiniDumpExplorer::DataGrid::AreRowGroupHeadersFrozenProperty(); } },
          { L"autogeneratecolumns"s, []() { return winrt::MiniDumpExplorer::DataGrid::AutoGenerateColumnsProperty(); } },
          { L"canuserreordercolumns"s, []() { return winrt::MiniDumpExplorer::DataGrid::CanUserReorderColumnsProperty(); } },
          { L"canuserresizecolumns"s, []() { return winrt::MiniDumpExplorer::DataGrid::CanUserResizeColumnsProperty(); } },
          { L"canusersortcolumns"s, []() { return winrt::MiniDumpExplorer::DataGrid::CanUserSortColumnsProperty(); } },
          { L"cellstyle"s, []() { return winrt::MiniDumpExplorer::DataGrid::CellStyleProperty(); } },
          { L"clipboardcopymode"s, []() { return winrt::MiniDumpExplorer::DataGrid::ClipboardCopyModeProperty(); } },
          { L"columnheaderheight"s, []() { return winrt::MiniDumpExplorer::DataGrid::ColumnHeaderHeightProperty(); } },
          { L"columnheaderstyle"s, []() { return winrt::MiniDumpExplorer::DataGrid::ColumnHeaderStyleProperty(); } },
          { L"columnwidth"s, []() { return winrt::MiniDumpExplorer::DataGrid::ColumnWidthProperty(); } },
          { L"datafetchsize"s, []() { return winrt::MiniDumpExplorer::DataGrid::DataFetchSizeProperty(); } },
          { L"dragindicatorstyle"s, []() { return winrt::MiniDumpExplorer::DataGrid::DragIndicatorStyleProperty(); } },
          { L"droplocationindicatorstyle"s, []() { return winrt::MiniDumpExplorer::DataGrid::DropLocationIndicatorStyleProperty(); } },
          { L"frozencolumncount"s, []() { return winrt::MiniDumpExplorer::DataGrid::FrozenColumnCountProperty(); } },
          { L"gridlinesvisibility"s, []() { return winrt::MiniDumpExplorer::DataGrid::GridLinesVisibilityProperty(); } },
          { L"headersvisibility"s, []() { return winrt::MiniDumpExplorer::DataGrid::HeadersVisibilityProperty(); } },
          { L"horizontalgridlinesbrush"s, []() { return winrt::MiniDumpExplorer::DataGrid::HorizontalGridLinesBrushProperty(); } },
          { L"horizontalscrollbarvisibility"s, []() { return winrt::MiniDumpExplorer::DataGrid::HorizontalScrollBarVisibilityProperty(); } },
          { L"isreadonly"s, []() { return winrt::MiniDumpExplorer::DataGrid::IsReadOnlyProperty(); } },
          { L"isvalid"s, []() { return winrt::MiniDumpExplorer::DataGrid::IsValidProperty(); } },
          { L"incrementalloadingthreshold"s, []() { return winrt::MiniDumpExplorer::DataGrid::IncrementalLoadingThresholdProperty(); } },
          { L"incrementalloadingtrigger"s, []() { return winrt::MiniDumpExplorer::DataGrid::IncrementalLoadingTriggerProperty(); } },
          { L"itemssource"s, []() { return winrt::MiniDumpExplorer::DataGrid::ItemsSourceProperty(); } },
          { L"maxcolumnwidth"s, []() { return winrt::MiniDumpExplorer::DataGrid::MaxColumnWidthProperty(); } },
          { L"mincolumnwidth"s, []() { return winrt::MiniDumpExplorer::DataGrid::MinColumnWidthProperty(); } },
          { L"rowbackground"s, []() { return winrt::MiniDumpExplorer::DataGrid::RowBackgroundProperty(); } },
          { L"rowdetailstemplate"s, []() { return winrt::MiniDumpExplorer::DataGrid::RowDetailsTemplateProperty(); } },
          { L"rowdetailsvisibilitymode"s, []() { return winrt::MiniDumpExplorer::DataGrid::RowDetailsVisibilityModeProperty(); } },
          { L"rowforeground"s, []() { return winrt::MiniDumpExplorer::DataGrid::RowForegroundProperty(); } },
          { L"rowheight"s, []() { return winrt::MiniDumpExplorer::DataGrid::RowHeightProperty(); } },
          { L"rowheaderwidth"s, []() { return winrt::MiniDumpExplorer::DataGrid::RowHeaderWidthProperty(); } },
          { L"rowheaderstyle"s, []() { return winrt::MiniDumpExplorer::DataGrid::RowHeaderStyleProperty(); } },
          { L"rowstyle"s, []() { return winrt::MiniDumpExplorer::DataGrid::RowStyleProperty(); } },
          { L"selectionmode"s, []() { return winrt::MiniDumpExplorer::DataGrid::SelectionModeProperty(); } },
          { L"selectedindex"s, []() { return winrt::MiniDumpExplorer::DataGrid::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::MiniDumpExplorer::DataGrid::SelectedItemProperty(); } },
          { L"verticalgridlinesbrush"s, []() { return winrt::MiniDumpExplorer::DataGrid::VerticalGridLinesBrushProperty(); } },
          { L"verticalscrollbarvisibility"s, []() { return winrt::MiniDumpExplorer::DataGrid::VerticalScrollBarVisibilityProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGridCell::IsValidProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGridCell>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"isvalid"s, []() { return winrt::MiniDumpExplorer::DataGridCell::IsValidProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGridColumnHeader::SeparatorBrushProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGridColumnHeader>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"separatorbrush"s, []() { return winrt::MiniDumpExplorer::DataGridColumnHeader::SeparatorBrushProperty(); } },
          { L"separatorvisibility"s, []() { return winrt::MiniDumpExplorer::DataGridColumnHeader::SeparatorVisibilityProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGridComboBoxColumn::ItemsSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGridComboBoxColumn>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"itemssource"s, []() { return winrt::MiniDumpExplorer::DataGridComboBoxColumn::ItemsSourceProperty(); } },
          { L"displaymemberpath"s, []() { return winrt::MiniDumpExplorer::DataGridComboBoxColumn::DisplayMemberPathProperty(); } },
          { L"fontfamily"s, []() { return winrt::MiniDumpExplorer::DataGridComboBoxColumn::FontFamilyProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGridDetailsPresenter::ContentHeightProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGridDetailsPresenter>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"contentheight"s, []() { return winrt::MiniDumpExplorer::DataGridDetailsPresenter::ContentHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGridFrozenGrid::IsFrozenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGridFrozenGrid>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"isfrozen"s, []() { return winrt::MiniDumpExplorer::DataGridFrozenGrid::IsFrozenProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGridRow::DetailsTemplateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGridRow>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"detailstemplate"s, []() { return winrt::MiniDumpExplorer::DataGridRow::DetailsTemplateProperty(); } },
          { L"detailsvisibility"s, []() { return winrt::MiniDumpExplorer::DataGridRow::DetailsVisibilityProperty(); } },
          { L"header"s, []() { return winrt::MiniDumpExplorer::DataGridRow::HeaderProperty(); } },
          { L"headerstyle"s, []() { return winrt::MiniDumpExplorer::DataGridRow::HeaderStyleProperty(); } },
          { L"isvalid"s, []() { return winrt::MiniDumpExplorer::DataGridRow::IsValidProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGridRowGroupHeader::HeaderStyleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGridRowGroupHeader>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"headerstyle"s, []() { return winrt::MiniDumpExplorer::DataGridRowGroupHeader::HeaderStyleProperty(); } },
          { L"itemcountvisibility"s, []() { return winrt::MiniDumpExplorer::DataGridRowGroupHeader::ItemCountVisibilityProperty(); } },
          { L"level"s, []() { return winrt::MiniDumpExplorer::DataGridRowGroupHeader::LevelProperty(); } },
          { L"propertyname"s, []() { return winrt::MiniDumpExplorer::DataGridRowGroupHeader::PropertyNameProperty(); } },
          { L"propertynamevisibility"s, []() { return winrt::MiniDumpExplorer::DataGridRowGroupHeader::PropertyNameVisibilityProperty(); } },
          { L"propertyvalue"s, []() { return winrt::MiniDumpExplorer::DataGridRowGroupHeader::PropertyValueProperty(); } },
          { L"sublevelindent"s, []() { return winrt::MiniDumpExplorer::DataGridRowGroupHeader::SublevelIndentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGridRowHeader::SeparatorBrushProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGridRowHeader>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"separatorbrush"s, []() { return winrt::MiniDumpExplorer::DataGridRowHeader::SeparatorBrushProperty(); } },
          { L"separatorvisibility"s, []() { return winrt::MiniDumpExplorer::DataGridRowHeader::SeparatorVisibilityProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::DataGridTextColumn::FontFamilyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::DataGridTextColumn>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"fontfamily"s, []() { return winrt::MiniDumpExplorer::DataGridTextColumn::FontFamilyProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::IsEqualStateTrigger::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::IsEqualStateTrigger>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"value"s, []() { return winrt::MiniDumpExplorer::IsEqualStateTrigger::ValueProperty(); } },
          { L"to"s, []() { return winrt::MiniDumpExplorer::IsEqualStateTrigger::ToProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::IsNullOrEmptyStateTrigger::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::IsNullOrEmptyStateTrigger>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"value"s, []() { return winrt::MiniDumpExplorer::IsNullOrEmptyStateTrigger::ValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::SettingsCard::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::SettingsCard>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"header"s, []() { return winrt::MiniDumpExplorer::SettingsCard::HeaderProperty(); } },
          { L"description"s, []() { return winrt::MiniDumpExplorer::SettingsCard::DescriptionProperty(); } },
          { L"headericon"s, []() { return winrt::MiniDumpExplorer::SettingsCard::HeaderIconProperty(); } },
          { L"actionicon"s, []() { return winrt::MiniDumpExplorer::SettingsCard::ActionIconProperty(); } },
          { L"actionicontooltip"s, []() { return winrt::MiniDumpExplorer::SettingsCard::ActionIconToolTipProperty(); } },
          { L"isclickenabled"s, []() { return winrt::MiniDumpExplorer::SettingsCard::IsClickEnabledProperty(); } },
          { L"contentalignment"s, []() { return winrt::MiniDumpExplorer::SettingsCard::ContentAlignmentProperty(); } },
          { L"isactioniconvisible"s, []() { return winrt::MiniDumpExplorer::SettingsCard::IsActionIconVisibleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::SettingsExpander::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::SettingsExpander>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"header"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::HeaderProperty(); } },
          { L"description"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::DescriptionProperty(); } },
          { L"headericon"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::HeaderIconProperty(); } },
          { L"itemsheader"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::ItemsHeaderProperty(); } },
          { L"itemsfooter"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::ItemsFooterProperty(); } },
          { L"isexpanded"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::IsExpandedProperty(); } },
          { L"items"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::ItemsProperty(); } },
          { L"itemssource"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::ItemsSourceProperty(); } },
          { L"itemtemplate"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::ItemTemplateProperty(); } },
          { L"itemcontainerstyleselector"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::ItemContainerStyleSelectorProperty(); } },
          { L"content"s, []() { return winrt::MiniDumpExplorer::SettingsExpander::ContentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::MiniDumpExplorer::StyleExtensions::ResourcesProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::MiniDumpExplorer::StyleExtensions>(),
      ObjectData {
        ObjectType::Application, 
        {
          { L"resources"s, []() { return winrt::MiniDumpExplorer::StyleExtensions::ResourcesProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::AdaptiveTrigger::MinWindowWidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::AdaptiveTrigger>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"minwindowwidth"s, []() { return winrt::Windows::UI::Xaml::AdaptiveTrigger::MinWindowWidthProperty(); } },
          { L"minwindowheight"s, []() { return winrt::Windows::UI::Xaml::AdaptiveTrigger::MinWindowHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::DataTemplate::ExtensionInstanceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::DataTemplate>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"extensioninstance"s, []() { return winrt::Windows::UI::Xaml::DataTemplate::ExtensionInstanceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::FrameworkElement::TagProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::FrameworkElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"tag"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::TagProperty(); } },
          { L"language"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::LanguageProperty(); } },
          { L"actualwidth"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::ActualWidthProperty(); } },
          { L"actualheight"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::ActualHeightProperty(); } },
          { L"width"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::WidthProperty(); } },
          { L"height"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::HeightProperty(); } },
          { L"minwidth"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::MinWidthProperty(); } },
          { L"maxwidth"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::MaxWidthProperty(); } },
          { L"minheight"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::MinHeightProperty(); } },
          { L"maxheight"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::MaxHeightProperty(); } },
          { L"horizontalalignment"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::HorizontalAlignmentProperty(); } },
          { L"verticalalignment"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::VerticalAlignmentProperty(); } },
          { L"margin"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::MarginProperty(); } },
          { L"name"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::NameProperty(); } },
          { L"datacontext"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::DataContextProperty(); } },
          { L"style"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::StyleProperty(); } },
          { L"flowdirection"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::FlowDirectionProperty(); } },
          { L"requestedtheme"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::RequestedThemeProperty(); } },
          { L"allowfocusoninteraction"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::AllowFocusOnInteractionProperty(); } },
          { L"focusvisualmargin"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::FocusVisualMarginProperty(); } },
          { L"focusvisualsecondarythickness"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::FocusVisualSecondaryThicknessProperty(); } },
          { L"focusvisualprimarythickness"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::FocusVisualPrimaryThicknessProperty(); } },
          { L"focusvisualsecondarybrush"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::FocusVisualSecondaryBrushProperty(); } },
          { L"focusvisualprimarybrush"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::FocusVisualPrimaryBrushProperty(); } },
          { L"allowfocuswhendisabled"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::AllowFocusWhenDisabledProperty(); } },
          { L"actualtheme"s, []() { return winrt::Windows::UI::Xaml::FrameworkElement::ActualThemeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::StateTrigger::IsActiveProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::StateTrigger>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isactive"s, []() { return winrt::Windows::UI::Xaml::StateTrigger::IsActiveProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::UIElement::AllowDropProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::UIElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"allowdrop"s, []() { return winrt::Windows::UI::Xaml::UIElement::AllowDropProperty(); } },
          { L"opacity"s, []() { return winrt::Windows::UI::Xaml::UIElement::OpacityProperty(); } },
          { L"clip"s, []() { return winrt::Windows::UI::Xaml::UIElement::ClipProperty(); } },
          { L"rendertransform"s, []() { return winrt::Windows::UI::Xaml::UIElement::RenderTransformProperty(); } },
          { L"projection"s, []() { return winrt::Windows::UI::Xaml::UIElement::ProjectionProperty(); } },
          { L"rendertransformorigin"s, []() { return winrt::Windows::UI::Xaml::UIElement::RenderTransformOriginProperty(); } },
          { L"ishittestvisible"s, []() { return winrt::Windows::UI::Xaml::UIElement::IsHitTestVisibleProperty(); } },
          { L"visibility"s, []() { return winrt::Windows::UI::Xaml::UIElement::VisibilityProperty(); } },
          { L"uselayoutrounding"s, []() { return winrt::Windows::UI::Xaml::UIElement::UseLayoutRoundingProperty(); } },
          { L"transitions"s, []() { return winrt::Windows::UI::Xaml::UIElement::TransitionsProperty(); } },
          { L"cachemode"s, []() { return winrt::Windows::UI::Xaml::UIElement::CacheModeProperty(); } },
          { L"istapenabled"s, []() { return winrt::Windows::UI::Xaml::UIElement::IsTapEnabledProperty(); } },
          { L"isdoubletapenabled"s, []() { return winrt::Windows::UI::Xaml::UIElement::IsDoubleTapEnabledProperty(); } },
          { L"isrighttapenabled"s, []() { return winrt::Windows::UI::Xaml::UIElement::IsRightTapEnabledProperty(); } },
          { L"isholdingenabled"s, []() { return winrt::Windows::UI::Xaml::UIElement::IsHoldingEnabledProperty(); } },
          { L"manipulationmode"s, []() { return winrt::Windows::UI::Xaml::UIElement::ManipulationModeProperty(); } },
          { L"pointercaptures"s, []() { return winrt::Windows::UI::Xaml::UIElement::PointerCapturesProperty(); } },
          { L"shadow"s, []() { return winrt::Windows::UI::Xaml::UIElement::ShadowProperty(); } },
          { L"compositemode"s, []() { return winrt::Windows::UI::Xaml::UIElement::CompositeModeProperty(); } },
          { L"transform3d"s, []() { return winrt::Windows::UI::Xaml::UIElement::Transform3DProperty(); } },
          { L"candrag"s, []() { return winrt::Windows::UI::Xaml::UIElement::CanDragProperty(); } },
          { L"contextflyout"s, []() { return winrt::Windows::UI::Xaml::UIElement::ContextFlyoutProperty(); } },
          { L"exitdisplaymodeonaccesskeyinvoked"s, []() { return winrt::Windows::UI::Xaml::UIElement::ExitDisplayModeOnAccessKeyInvokedProperty(); } },
          { L"isaccesskeyscope"s, []() { return winrt::Windows::UI::Xaml::UIElement::IsAccessKeyScopeProperty(); } },
          { L"accesskeyscopeowner"s, []() { return winrt::Windows::UI::Xaml::UIElement::AccessKeyScopeOwnerProperty(); } },
          { L"accesskey"s, []() { return winrt::Windows::UI::Xaml::UIElement::AccessKeyProperty(); } },
          { L"lights"s, []() { return winrt::Windows::UI::Xaml::UIElement::LightsProperty(); } },
          { L"keytipplacementmode"s, []() { return winrt::Windows::UI::Xaml::UIElement::KeyTipPlacementModeProperty(); } },
          { L"keytiphorizontaloffset"s, []() { return winrt::Windows::UI::Xaml::UIElement::KeyTipHorizontalOffsetProperty(); } },
          { L"keytipverticaloffset"s, []() { return winrt::Windows::UI::Xaml::UIElement::KeyTipVerticalOffsetProperty(); } },
          { L"xyfocuskeyboardnavigation"s, []() { return winrt::Windows::UI::Xaml::UIElement::XYFocusKeyboardNavigationProperty(); } },
          { L"xyfocusupnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::UIElement::XYFocusUpNavigationStrategyProperty(); } },
          { L"xyfocusdownnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::UIElement::XYFocusDownNavigationStrategyProperty(); } },
          { L"xyfocusleftnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::UIElement::XYFocusLeftNavigationStrategyProperty(); } },
          { L"xyfocusrightnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::UIElement::XYFocusRightNavigationStrategyProperty(); } },
          { L"highcontrastadjustment"s, []() { return winrt::Windows::UI::Xaml::UIElement::HighContrastAdjustmentProperty(); } },
          { L"tabfocusnavigation"s, []() { return winrt::Windows::UI::Xaml::UIElement::TabFocusNavigationProperty(); } },
          { L"keytiptarget"s, []() { return winrt::Windows::UI::Xaml::UIElement::KeyTipTargetProperty(); } },
          { L"keyboardacceleratorplacementtarget"s, []() { return winrt::Windows::UI::Xaml::UIElement::KeyboardAcceleratorPlacementTargetProperty(); } },
          { L"keyboardacceleratorplacementmode"s, []() { return winrt::Windows::UI::Xaml::UIElement::KeyboardAcceleratorPlacementModeProperty(); } },
          { L"canbescrollanchor"s, []() { return winrt::Windows::UI::Xaml::UIElement::CanBeScrollAnchorProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::VisualStateManager::CustomVisualStateManagerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::VisualStateManager>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"customvisualstatemanager"s, []() { return winrt::Windows::UI::Xaml::VisualStateManager::CustomVisualStateManagerProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Automation.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::AnnotationPatternIdentifiers::AnnotationTypeIdProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::AnnotationPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"annotationtypeid"s, []() { return winrt::Windows::UI::Xaml::Automation::AnnotationPatternIdentifiers::AnnotationTypeIdProperty(); } },
          { L"annotationtypename"s, []() { return winrt::Windows::UI::Xaml::Automation::AnnotationPatternIdentifiers::AnnotationTypeNameProperty(); } },
          { L"author"s, []() { return winrt::Windows::UI::Xaml::Automation::AnnotationPatternIdentifiers::AuthorProperty(); } },
          { L"datetime"s, []() { return winrt::Windows::UI::Xaml::Automation::AnnotationPatternIdentifiers::DateTimeProperty(); } },
          { L"target"s, []() { return winrt::Windows::UI::Xaml::Automation::AnnotationPatternIdentifiers::TargetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::AutomationAnnotation::TypeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::AutomationAnnotation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"type"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationAnnotation::TypeProperty(); } },
          { L"element"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationAnnotation::ElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::AcceleratorKeyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"acceleratorkey"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::AcceleratorKeyProperty(); } },
          { L"accesskey"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::AccessKeyProperty(); } },
          { L"automationid"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::AutomationIdProperty(); } },
          { L"boundingrectangle"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::BoundingRectangleProperty(); } },
          { L"classname"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::ClassNameProperty(); } },
          { L"clickablepoint"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::ClickablePointProperty(); } },
          { L"controltype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::ControlTypeProperty(); } },
          { L"haskeyboardfocus"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::HasKeyboardFocusProperty(); } },
          { L"helptext"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::HelpTextProperty(); } },
          { L"iscontentelement"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsContentElementProperty(); } },
          { L"iscontrolelement"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsControlElementProperty(); } },
          { L"isenabled"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsEnabledProperty(); } },
          { L"iskeyboardfocusable"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsKeyboardFocusableProperty(); } },
          { L"isoffscreen"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsOffscreenProperty(); } },
          { L"ispassword"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsPasswordProperty(); } },
          { L"isrequiredforform"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsRequiredForFormProperty(); } },
          { L"itemstatus"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::ItemStatusProperty(); } },
          { L"itemtype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::ItemTypeProperty(); } },
          { L"labeledby"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::LabeledByProperty(); } },
          { L"localizedcontroltype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::LocalizedControlTypeProperty(); } },
          { L"name"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::NameProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::OrientationProperty(); } },
          { L"livesetting"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::LiveSettingProperty(); } },
          { L"controlledpeers"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::ControlledPeersProperty(); } },
          { L"positioninset"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::PositionInSetProperty(); } },
          { L"sizeofset"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::SizeOfSetProperty(); } },
          { L"level"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::LevelProperty(); } },
          { L"annotations"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::AnnotationsProperty(); } },
          { L"landmarktype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::LandmarkTypeProperty(); } },
          { L"localizedlandmarktype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::LocalizedLandmarkTypeProperty(); } },
          { L"isperipheral"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsPeripheralProperty(); } },
          { L"isdatavalidforform"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsDataValidForFormProperty(); } },
          { L"fulldescription"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::FullDescriptionProperty(); } },
          { L"describedby"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::DescribedByProperty(); } },
          { L"flowsto"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::FlowsToProperty(); } },
          { L"flowsfrom"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::FlowsFromProperty(); } },
          { L"culture"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::CultureProperty(); } },
          { L"headinglevel"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::HeadingLevelProperty(); } },
          { L"isdialog"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationElementIdentifiers::IsDialogProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::AutomationProperties::AcceleratorKeyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::AutomationProperties>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"acceleratorkey"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::AcceleratorKeyProperty(); } },
          { L"accesskey"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::AccessKeyProperty(); } },
          { L"automationid"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::AutomationIdProperty(); } },
          { L"helptext"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::HelpTextProperty(); } },
          { L"isrequiredforform"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::IsRequiredForFormProperty(); } },
          { L"itemstatus"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::ItemStatusProperty(); } },
          { L"itemtype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::ItemTypeProperty(); } },
          { L"labeledby"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::LabeledByProperty(); } },
          { L"name"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::NameProperty(); } },
          { L"livesetting"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::LiveSettingProperty(); } },
          { L"accessibilityview"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::AccessibilityViewProperty(); } },
          { L"controlledpeers"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::ControlledPeersProperty(); } },
          { L"positioninset"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::PositionInSetProperty(); } },
          { L"sizeofset"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::SizeOfSetProperty(); } },
          { L"level"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::LevelProperty(); } },
          { L"annotations"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::AnnotationsProperty(); } },
          { L"landmarktype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::LandmarkTypeProperty(); } },
          { L"localizedlandmarktype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::LocalizedLandmarkTypeProperty(); } },
          { L"isperipheral"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::IsPeripheralProperty(); } },
          { L"isdatavalidforform"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::IsDataValidForFormProperty(); } },
          { L"fulldescription"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::FullDescriptionProperty(); } },
          { L"localizedcontroltype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::LocalizedControlTypeProperty(); } },
          { L"describedby"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::DescribedByProperty(); } },
          { L"flowsto"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::FlowsToProperty(); } },
          { L"flowsfrom"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::FlowsFromProperty(); } },
          { L"culture"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::CultureProperty(); } },
          { L"headinglevel"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::HeadingLevelProperty(); } },
          { L"isdialog"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::IsDialogProperty(); } },
          { L"automationcontroltype"s, []() { return winrt::Windows::UI::Xaml::Automation::AutomationProperties::AutomationControlTypeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::DockPatternIdentifiers::DockPositionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::DockPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"dockposition"s, []() { return winrt::Windows::UI::Xaml::Automation::DockPatternIdentifiers::DockPositionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::DragPatternIdentifiers::DropEffectProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::DragPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"dropeffect"s, []() { return winrt::Windows::UI::Xaml::Automation::DragPatternIdentifiers::DropEffectProperty(); } },
          { L"dropeffects"s, []() { return winrt::Windows::UI::Xaml::Automation::DragPatternIdentifiers::DropEffectsProperty(); } },
          { L"grabbeditems"s, []() { return winrt::Windows::UI::Xaml::Automation::DragPatternIdentifiers::GrabbedItemsProperty(); } },
          { L"isgrabbed"s, []() { return winrt::Windows::UI::Xaml::Automation::DragPatternIdentifiers::IsGrabbedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::DropTargetPatternIdentifiers::DropTargetEffectProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::DropTargetPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"droptargeteffect"s, []() { return winrt::Windows::UI::Xaml::Automation::DropTargetPatternIdentifiers::DropTargetEffectProperty(); } },
          { L"droptargeteffects"s, []() { return winrt::Windows::UI::Xaml::Automation::DropTargetPatternIdentifiers::DropTargetEffectsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::ExpandCollapsePatternIdentifiers::ExpandCollapseStateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::ExpandCollapsePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"expandcollapsestate"s, []() { return winrt::Windows::UI::Xaml::Automation::ExpandCollapsePatternIdentifiers::ExpandCollapseStateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::GridItemPatternIdentifiers::ColumnProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::GridItemPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"column"s, []() { return winrt::Windows::UI::Xaml::Automation::GridItemPatternIdentifiers::ColumnProperty(); } },
          { L"columnspan"s, []() { return winrt::Windows::UI::Xaml::Automation::GridItemPatternIdentifiers::ColumnSpanProperty(); } },
          { L"containinggrid"s, []() { return winrt::Windows::UI::Xaml::Automation::GridItemPatternIdentifiers::ContainingGridProperty(); } },
          { L"row"s, []() { return winrt::Windows::UI::Xaml::Automation::GridItemPatternIdentifiers::RowProperty(); } },
          { L"rowspan"s, []() { return winrt::Windows::UI::Xaml::Automation::GridItemPatternIdentifiers::RowSpanProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::GridPatternIdentifiers::ColumnCountProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::GridPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"columncount"s, []() { return winrt::Windows::UI::Xaml::Automation::GridPatternIdentifiers::ColumnCountProperty(); } },
          { L"rowcount"s, []() { return winrt::Windows::UI::Xaml::Automation::GridPatternIdentifiers::RowCountProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::MultipleViewPatternIdentifiers::CurrentViewProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::MultipleViewPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"currentview"s, []() { return winrt::Windows::UI::Xaml::Automation::MultipleViewPatternIdentifiers::CurrentViewProperty(); } },
          { L"supportedviews"s, []() { return winrt::Windows::UI::Xaml::Automation::MultipleViewPatternIdentifiers::SupportedViewsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::RangeValuePatternIdentifiers::IsReadOnlyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::RangeValuePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isreadonly"s, []() { return winrt::Windows::UI::Xaml::Automation::RangeValuePatternIdentifiers::IsReadOnlyProperty(); } },
          { L"largechange"s, []() { return winrt::Windows::UI::Xaml::Automation::RangeValuePatternIdentifiers::LargeChangeProperty(); } },
          { L"maximum"s, []() { return winrt::Windows::UI::Xaml::Automation::RangeValuePatternIdentifiers::MaximumProperty(); } },
          { L"minimum"s, []() { return winrt::Windows::UI::Xaml::Automation::RangeValuePatternIdentifiers::MinimumProperty(); } },
          { L"smallchange"s, []() { return winrt::Windows::UI::Xaml::Automation::RangeValuePatternIdentifiers::SmallChangeProperty(); } },
          { L"value"s, []() { return winrt::Windows::UI::Xaml::Automation::RangeValuePatternIdentifiers::ValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::ScrollPatternIdentifiers::HorizontallyScrollableProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::ScrollPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"horizontallyscrollable"s, []() { return winrt::Windows::UI::Xaml::Automation::ScrollPatternIdentifiers::HorizontallyScrollableProperty(); } },
          { L"horizontalscrollpercent"s, []() { return winrt::Windows::UI::Xaml::Automation::ScrollPatternIdentifiers::HorizontalScrollPercentProperty(); } },
          { L"horizontalviewsize"s, []() { return winrt::Windows::UI::Xaml::Automation::ScrollPatternIdentifiers::HorizontalViewSizeProperty(); } },
          { L"verticallyscrollable"s, []() { return winrt::Windows::UI::Xaml::Automation::ScrollPatternIdentifiers::VerticallyScrollableProperty(); } },
          { L"verticalscrollpercent"s, []() { return winrt::Windows::UI::Xaml::Automation::ScrollPatternIdentifiers::VerticalScrollPercentProperty(); } },
          { L"verticalviewsize"s, []() { return winrt::Windows::UI::Xaml::Automation::ScrollPatternIdentifiers::VerticalViewSizeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::SelectionItemPatternIdentifiers::IsSelectedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::SelectionItemPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isselected"s, []() { return winrt::Windows::UI::Xaml::Automation::SelectionItemPatternIdentifiers::IsSelectedProperty(); } },
          { L"selectioncontainer"s, []() { return winrt::Windows::UI::Xaml::Automation::SelectionItemPatternIdentifiers::SelectionContainerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::SelectionPatternIdentifiers::CanSelectMultipleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::SelectionPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"canselectmultiple"s, []() { return winrt::Windows::UI::Xaml::Automation::SelectionPatternIdentifiers::CanSelectMultipleProperty(); } },
          { L"isselectionrequired"s, []() { return winrt::Windows::UI::Xaml::Automation::SelectionPatternIdentifiers::IsSelectionRequiredProperty(); } },
          { L"selection"s, []() { return winrt::Windows::UI::Xaml::Automation::SelectionPatternIdentifiers::SelectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::SpreadsheetItemPatternIdentifiers::FormulaProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::SpreadsheetItemPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"formula"s, []() { return winrt::Windows::UI::Xaml::Automation::SpreadsheetItemPatternIdentifiers::FormulaProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::StylesPatternIdentifiers::ExtendedPropertiesProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::StylesPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"extendedproperties"s, []() { return winrt::Windows::UI::Xaml::Automation::StylesPatternIdentifiers::ExtendedPropertiesProperty(); } },
          { L"fillcolor"s, []() { return winrt::Windows::UI::Xaml::Automation::StylesPatternIdentifiers::FillColorProperty(); } },
          { L"fillpatterncolor"s, []() { return winrt::Windows::UI::Xaml::Automation::StylesPatternIdentifiers::FillPatternColorProperty(); } },
          { L"fillpatternstyle"s, []() { return winrt::Windows::UI::Xaml::Automation::StylesPatternIdentifiers::FillPatternStyleProperty(); } },
          { L"shape"s, []() { return winrt::Windows::UI::Xaml::Automation::StylesPatternIdentifiers::ShapeProperty(); } },
          { L"styleid"s, []() { return winrt::Windows::UI::Xaml::Automation::StylesPatternIdentifiers::StyleIdProperty(); } },
          { L"stylename"s, []() { return winrt::Windows::UI::Xaml::Automation::StylesPatternIdentifiers::StyleNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::TableItemPatternIdentifiers::ColumnHeaderItemsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::TableItemPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"columnheaderitems"s, []() { return winrt::Windows::UI::Xaml::Automation::TableItemPatternIdentifiers::ColumnHeaderItemsProperty(); } },
          { L"rowheaderitems"s, []() { return winrt::Windows::UI::Xaml::Automation::TableItemPatternIdentifiers::RowHeaderItemsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::TablePatternIdentifiers::ColumnHeadersProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::TablePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"columnheaders"s, []() { return winrt::Windows::UI::Xaml::Automation::TablePatternIdentifiers::ColumnHeadersProperty(); } },
          { L"rowheaders"s, []() { return winrt::Windows::UI::Xaml::Automation::TablePatternIdentifiers::RowHeadersProperty(); } },
          { L"roworcolumnmajor"s, []() { return winrt::Windows::UI::Xaml::Automation::TablePatternIdentifiers::RowOrColumnMajorProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::TogglePatternIdentifiers::ToggleStateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::TogglePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"togglestate"s, []() { return winrt::Windows::UI::Xaml::Automation::TogglePatternIdentifiers::ToggleStateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::TransformPattern2Identifiers::CanZoomProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::TransformPattern2Identifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"canzoom"s, []() { return winrt::Windows::UI::Xaml::Automation::TransformPattern2Identifiers::CanZoomProperty(); } },
          { L"zoomlevel"s, []() { return winrt::Windows::UI::Xaml::Automation::TransformPattern2Identifiers::ZoomLevelProperty(); } },
          { L"maxzoom"s, []() { return winrt::Windows::UI::Xaml::Automation::TransformPattern2Identifiers::MaxZoomProperty(); } },
          { L"minzoom"s, []() { return winrt::Windows::UI::Xaml::Automation::TransformPattern2Identifiers::MinZoomProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::TransformPatternIdentifiers::CanMoveProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::TransformPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"canmove"s, []() { return winrt::Windows::UI::Xaml::Automation::TransformPatternIdentifiers::CanMoveProperty(); } },
          { L"canresize"s, []() { return winrt::Windows::UI::Xaml::Automation::TransformPatternIdentifiers::CanResizeProperty(); } },
          { L"canrotate"s, []() { return winrt::Windows::UI::Xaml::Automation::TransformPatternIdentifiers::CanRotateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::ValuePatternIdentifiers::IsReadOnlyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::ValuePatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isreadonly"s, []() { return winrt::Windows::UI::Xaml::Automation::ValuePatternIdentifiers::IsReadOnlyProperty(); } },
          { L"value"s, []() { return winrt::Windows::UI::Xaml::Automation::ValuePatternIdentifiers::ValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::WindowPatternIdentifiers::CanMaximizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::WindowPatternIdentifiers>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"canmaximize"s, []() { return winrt::Windows::UI::Xaml::Automation::WindowPatternIdentifiers::CanMaximizeProperty(); } },
          { L"canminimize"s, []() { return winrt::Windows::UI::Xaml::Automation::WindowPatternIdentifiers::CanMinimizeProperty(); } },
          { L"ismodal"s, []() { return winrt::Windows::UI::Xaml::Automation::WindowPatternIdentifiers::IsModalProperty(); } },
          { L"istopmost"s, []() { return winrt::Windows::UI::Xaml::Automation::WindowPatternIdentifiers::IsTopmostProperty(); } },
          { L"windowinteractionstate"s, []() { return winrt::Windows::UI::Xaml::Automation::WindowPatternIdentifiers::WindowInteractionStateProperty(); } },
          { L"windowvisualstate"s, []() { return winrt::Windows::UI::Xaml::Automation::WindowPatternIdentifiers::WindowVisualStateProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Automation.Peers.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeerAnnotation::TypeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeerAnnotation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"type"s, []() { return winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeerAnnotation::TypeProperty(); } },
          { L"peer"s, []() { return winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeerAnnotation::PeerProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Controls.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::AppBar::IsOpenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::AppBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isopen"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBar::IsOpenProperty(); } },
          { L"issticky"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBar::IsStickyProperty(); } },
          { L"closeddisplaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBar::ClosedDisplayModeProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBar::LightDismissOverlayModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::AppBarButton::LabelProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::AppBarButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"label"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarButton::LabelProperty(); } },
          { L"icon"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarButton::IconProperty(); } },
          { L"iscompact"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarButton::IsCompactProperty(); } },
          { L"labelposition"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarButton::LabelPositionProperty(); } },
          { L"isinoverflow"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarButton::IsInOverflowProperty(); } },
          { L"dynamicoverfloworder"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarButton::DynamicOverflowOrderProperty(); } },
          { L"keyboardacceleratortextoverride"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarButton::KeyboardAcceleratorTextOverrideProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::AppBarElementContainer::IsCompactProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::AppBarElementContainer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iscompact"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarElementContainer::IsCompactProperty(); } },
          { L"isinoverflow"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarElementContainer::IsInOverflowProperty(); } },
          { L"dynamicoverfloworder"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarElementContainer::DynamicOverflowOrderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::AppBarSeparator::IsCompactProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::AppBarSeparator>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iscompact"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarSeparator::IsCompactProperty(); } },
          { L"isinoverflow"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarSeparator::IsInOverflowProperty(); } },
          { L"dynamicoverfloworder"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarSeparator::DynamicOverflowOrderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::AppBarToggleButton::LabelProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::AppBarToggleButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"label"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarToggleButton::LabelProperty(); } },
          { L"icon"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarToggleButton::IconProperty(); } },
          { L"iscompact"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarToggleButton::IsCompactProperty(); } },
          { L"labelposition"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarToggleButton::LabelPositionProperty(); } },
          { L"isinoverflow"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarToggleButton::IsInOverflowProperty(); } },
          { L"dynamicoverfloworder"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarToggleButton::DynamicOverflowOrderProperty(); } },
          { L"keyboardacceleratortextoverride"s, []() { return winrt::Windows::UI::Xaml::Controls::AppBarToggleButton::KeyboardAcceleratorTextOverrideProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::MaxSuggestionListHeightProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::AutoSuggestBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"maxsuggestionlistheight"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::MaxSuggestionListHeightProperty(); } },
          { L"issuggestionlistopen"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::IsSuggestionListOpenProperty(); } },
          { L"textmemberpath"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::TextMemberPathProperty(); } },
          { L"text"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::TextProperty(); } },
          { L"updatetextonselect"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::UpdateTextOnSelectProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::PlaceholderTextProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::HeaderProperty(); } },
          { L"automaximizesuggestionarea"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::AutoMaximizeSuggestionAreaProperty(); } },
          { L"textboxstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::TextBoxStyleProperty(); } },
          { L"queryicon"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::QueryIconProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::LightDismissOverlayModeProperty(); } },
          { L"description"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBox::DescriptionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs::ReasonProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"reason"s, []() { return winrt::Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs::ReasonProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::BitmapIcon::UriSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::BitmapIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"urisource"s, []() { return winrt::Windows::UI::Xaml::Controls::BitmapIcon::UriSourceProperty(); } },
          { L"showasmonochrome"s, []() { return winrt::Windows::UI::Xaml::Controls::BitmapIcon::ShowAsMonochromeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::BitmapIconSource::UriSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::BitmapIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"urisource"s, []() { return winrt::Windows::UI::Xaml::Controls::BitmapIconSource::UriSourceProperty(); } },
          { L"showasmonochrome"s, []() { return winrt::Windows::UI::Xaml::Controls::BitmapIconSource::ShowAsMonochromeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Border::BorderBrushProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Border>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"borderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Border::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::Border::BorderThicknessProperty(); } },
          { L"background"s, []() { return winrt::Windows::UI::Xaml::Controls::Border::BackgroundProperty(); } },
          { L"cornerradius"s, []() { return winrt::Windows::UI::Xaml::Controls::Border::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::Border::PaddingProperty(); } },
          { L"childtransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::Border::ChildTransitionsProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Windows::UI::Xaml::Controls::Border::BackgroundSizingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Button::FlyoutProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Button>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"flyout"s, []() { return winrt::Windows::UI::Xaml::Controls::Button::FlyoutProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::DateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::CalendarDatePicker>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"date"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::DateProperty(); } },
          { L"iscalendaropen"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::IsCalendarOpenProperty(); } },
          { L"dateformat"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::DateFormatProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::PlaceholderTextProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::HeaderTemplateProperty(); } },
          { L"calendarviewstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::CalendarViewStyleProperty(); } },
          { L"mindate"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::MinDateProperty(); } },
          { L"maxdate"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::MaxDateProperty(); } },
          { L"istodayhighlighted"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::IsTodayHighlightedProperty(); } },
          { L"displaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::DisplayModeProperty(); } },
          { L"firstdayofweek"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::FirstDayOfWeekProperty(); } },
          { L"dayofweekformat"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::DayOfWeekFormatProperty(); } },
          { L"calendaridentifier"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::CalendarIdentifierProperty(); } },
          { L"isoutofscopeenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::IsOutOfScopeEnabledProperty(); } },
          { L"isgrouplabelvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::IsGroupLabelVisibleProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::LightDismissOverlayModeProperty(); } },
          { L"description"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarDatePicker::DescriptionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarIdentifierProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::CalendarView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"calendaridentifier"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarIdentifierProperty(); } },
          { L"dayofweekformat"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::DayOfWeekFormatProperty(); } },
          { L"isgrouplabelvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::IsGroupLabelVisibleProperty(); } },
          { L"displaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::DisplayModeProperty(); } },
          { L"firstdayofweek"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstDayOfWeekProperty(); } },
          { L"isoutofscopeenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::IsOutOfScopeEnabledProperty(); } },
          { L"istodayhighlighted"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::IsTodayHighlightedProperty(); } },
          { L"maxdate"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::MaxDateProperty(); } },
          { L"mindate"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::MinDateProperty(); } },
          { L"numberofweeksinview"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::NumberOfWeeksInViewProperty(); } },
          { L"selecteddates"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectedDatesProperty(); } },
          { L"selectionmode"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectionModeProperty(); } },
          { L"templatesettings"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TemplateSettingsProperty(); } },
          { L"focusborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FocusBorderBrushProperty(); } },
          { L"selectedhoverborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectedHoverBorderBrushProperty(); } },
          { L"selectedpressedborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectedPressedBorderBrushProperty(); } },
          { L"selectedborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectedBorderBrushProperty(); } },
          { L"hoverborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::HoverBorderBrushProperty(); } },
          { L"pressedborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::PressedBorderBrushProperty(); } },
          { L"calendaritemborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarItemBorderBrushProperty(); } },
          { L"outofscopebackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::OutOfScopeBackgroundProperty(); } },
          { L"calendaritembackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarItemBackgroundProperty(); } },
          { L"pressedforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::PressedForegroundProperty(); } },
          { L"todayforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TodayForegroundProperty(); } },
          { L"blackoutforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::BlackoutForegroundProperty(); } },
          { L"selectedforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectedForegroundProperty(); } },
          { L"outofscopeforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::OutOfScopeForegroundProperty(); } },
          { L"calendaritemforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarItemForegroundProperty(); } },
          { L"dayitemfontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::DayItemFontFamilyProperty(); } },
          { L"dayitemfontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::DayItemFontSizeProperty(); } },
          { L"dayitemfontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::DayItemFontStyleProperty(); } },
          { L"dayitemfontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::DayItemFontWeightProperty(); } },
          { L"todayfontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TodayFontWeightProperty(); } },
          { L"firstofmonthlabelfontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelFontFamilyProperty(); } },
          { L"firstofmonthlabelfontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelFontSizeProperty(); } },
          { L"firstofmonthlabelfontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelFontStyleProperty(); } },
          { L"firstofmonthlabelfontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelFontWeightProperty(); } },
          { L"monthyearitemfontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::MonthYearItemFontFamilyProperty(); } },
          { L"monthyearitemfontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::MonthYearItemFontSizeProperty(); } },
          { L"monthyearitemfontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::MonthYearItemFontStyleProperty(); } },
          { L"monthyearitemfontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::MonthYearItemFontWeightProperty(); } },
          { L"firstofyeardecadelabelfontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelFontFamilyProperty(); } },
          { L"firstofyeardecadelabelfontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelFontSizeProperty(); } },
          { L"firstofyeardecadelabelfontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelFontStyleProperty(); } },
          { L"firstofyeardecadelabelfontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelFontWeightProperty(); } },
          { L"horizontaldayitemalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::HorizontalDayItemAlignmentProperty(); } },
          { L"verticaldayitemalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::VerticalDayItemAlignmentProperty(); } },
          { L"horizontalfirstofmonthlabelalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::HorizontalFirstOfMonthLabelAlignmentProperty(); } },
          { L"verticalfirstofmonthlabelalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::VerticalFirstOfMonthLabelAlignmentProperty(); } },
          { L"calendaritemborderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarItemBorderThicknessProperty(); } },
          { L"calendarviewdayitemstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarViewDayItemStyleProperty(); } },
          { L"selecteddisabledborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectedDisabledBorderBrushProperty(); } },
          { L"todayselectedinnerborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TodaySelectedInnerBorderBrushProperty(); } },
          { L"blackoutstrikethroughbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::BlackoutStrikethroughBrushProperty(); } },
          { L"blackoutbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::BlackoutBackgroundProperty(); } },
          { L"calendaritemhoverbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarItemHoverBackgroundProperty(); } },
          { L"calendaritempressedbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarItemPressedBackgroundProperty(); } },
          { L"calendaritemdisabledbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarItemDisabledBackgroundProperty(); } },
          { L"todaybackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TodayBackgroundProperty(); } },
          { L"todayblackoutbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TodayBlackoutBackgroundProperty(); } },
          { L"todayhoverbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TodayHoverBackgroundProperty(); } },
          { L"todaypressedbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TodayPressedBackgroundProperty(); } },
          { L"todaydisabledbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TodayDisabledBackgroundProperty(); } },
          { L"todayblackoutforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::TodayBlackoutForegroundProperty(); } },
          { L"selectedhoverforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectedHoverForegroundProperty(); } },
          { L"selectedpressedforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectedPressedForegroundProperty(); } },
          { L"selecteddisabledforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::SelectedDisabledForegroundProperty(); } },
          { L"outofscopehoverforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::OutOfScopeHoverForegroundProperty(); } },
          { L"outofscopepressedforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::OutOfScopePressedForegroundProperty(); } },
          { L"disabledforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::DisabledForegroundProperty(); } },
          { L"dayitemmargin"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::DayItemMarginProperty(); } },
          { L"monthyearitemmargin"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::MonthYearItemMarginProperty(); } },
          { L"firstofmonthlabelmargin"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfMonthLabelMarginProperty(); } },
          { L"firstofyeardecadelabelmargin"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::FirstOfYearDecadeLabelMarginProperty(); } },
          { L"calendaritemcornerradius"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarView::CalendarItemCornerRadiusProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::CalendarViewDayItem::IsBlackoutProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::CalendarViewDayItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isblackout"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarViewDayItem::IsBlackoutProperty(); } },
          { L"date"s, []() { return winrt::Windows::UI::Xaml::Controls::CalendarViewDayItem::DateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Canvas::LeftProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Canvas>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"left"s, []() { return winrt::Windows::UI::Xaml::Controls::Canvas::LeftProperty(); } },
          { L"top"s, []() { return winrt::Windows::UI::Xaml::Controls::Canvas::TopProperty(); } },
          { L"zindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Canvas::ZIndexProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::CaptureElement::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::CaptureElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Windows::UI::Xaml::Controls::CaptureElement::SourceProperty(); } },
          { L"stretch"s, []() { return winrt::Windows::UI::Xaml::Controls::CaptureElement::StretchProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ColorPicker::ColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ColorPicker>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"color"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::ColorProperty(); } },
          { L"previouscolor"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::PreviousColorProperty(); } },
          { L"isalphaenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::IsAlphaEnabledProperty(); } },
          { L"iscolorspectrumvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::IsColorSpectrumVisibleProperty(); } },
          { L"iscolorpreviewvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::IsColorPreviewVisibleProperty(); } },
          { L"iscolorslidervisible"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::IsColorSliderVisibleProperty(); } },
          { L"isalphaslidervisible"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::IsAlphaSliderVisibleProperty(); } },
          { L"ismorebuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::IsMoreButtonVisibleProperty(); } },
          { L"iscolorchanneltextinputvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::IsColorChannelTextInputVisibleProperty(); } },
          { L"isalphatextinputvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::IsAlphaTextInputVisibleProperty(); } },
          { L"ishexinputvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::IsHexInputVisibleProperty(); } },
          { L"minhue"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::MinHueProperty(); } },
          { L"maxhue"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::MaxHueProperty(); } },
          { L"minsaturation"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::MinSaturationProperty(); } },
          { L"maxsaturation"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::MaxSaturationProperty(); } },
          { L"minvalue"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::MinValueProperty(); } },
          { L"maxvalue"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::MaxValueProperty(); } },
          { L"colorspectrumshape"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::ColorSpectrumShapeProperty(); } },
          { L"colorspectrumcomponents"s, []() { return winrt::Windows::UI::Xaml::Controls::ColorPicker::ColorSpectrumComponentsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ColumnDefinition::WidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ColumnDefinition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"width"s, []() { return winrt::Windows::UI::Xaml::Controls::ColumnDefinition::WidthProperty(); } },
          { L"maxwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::ColumnDefinition::MaxWidthProperty(); } },
          { L"minwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::ColumnDefinition::MinWidthProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ComboBox::IsDropDownOpenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ComboBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdropdownopen"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::IsDropDownOpenProperty(); } },
          { L"maxdropdownheight"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::MaxDropDownHeightProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::HeaderTemplateProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::PlaceholderTextProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::LightDismissOverlayModeProperty(); } },
          { L"istextsearchenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::IsTextSearchEnabledProperty(); } },
          { L"selectionchangedtrigger"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::SelectionChangedTriggerProperty(); } },
          { L"placeholderforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::PlaceholderForegroundProperty(); } },
          { L"iseditable"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::IsEditableProperty(); } },
          { L"text"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::TextProperty(); } },
          { L"textboxstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::TextBoxStyleProperty(); } },
          { L"description"s, []() { return winrt::Windows::UI::Xaml::Controls::ComboBox::DescriptionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::CommandBar::PrimaryCommandsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::CommandBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"primarycommands"s, []() { return winrt::Windows::UI::Xaml::Controls::CommandBar::PrimaryCommandsProperty(); } },
          { L"secondarycommands"s, []() { return winrt::Windows::UI::Xaml::Controls::CommandBar::SecondaryCommandsProperty(); } },
          { L"commandbaroverflowpresenterstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::CommandBar::CommandBarOverflowPresenterStyleProperty(); } },
          { L"defaultlabelposition"s, []() { return winrt::Windows::UI::Xaml::Controls::CommandBar::DefaultLabelPositionProperty(); } },
          { L"overflowbuttonvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::CommandBar::OverflowButtonVisibilityProperty(); } },
          { L"isdynamicoverflowenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::CommandBar::IsDynamicOverflowEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ContentControl::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ContentControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentControl::ContentProperty(); } },
          { L"contenttemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentControl::ContentTemplateProperty(); } },
          { L"contenttemplateselector"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentControl::ContentTemplateSelectorProperty(); } },
          { L"contenttransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentControl::ContentTransitionsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ContentDialog::TitleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ContentDialog>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"title"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::TitleProperty(); } },
          { L"titletemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::TitleTemplateProperty(); } },
          { L"fullsizedesired"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::FullSizeDesiredProperty(); } },
          { L"primarybuttontext"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::PrimaryButtonTextProperty(); } },
          { L"secondarybuttontext"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::SecondaryButtonTextProperty(); } },
          { L"primarybuttoncommand"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::PrimaryButtonCommandProperty(); } },
          { L"secondarybuttoncommand"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::SecondaryButtonCommandProperty(); } },
          { L"primarybuttoncommandparameter"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::PrimaryButtonCommandParameterProperty(); } },
          { L"secondarybuttoncommandparameter"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::SecondaryButtonCommandParameterProperty(); } },
          { L"isprimarybuttonenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::IsPrimaryButtonEnabledProperty(); } },
          { L"issecondarybuttonenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::IsSecondaryButtonEnabledProperty(); } },
          { L"closebuttontext"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::CloseButtonTextProperty(); } },
          { L"closebuttoncommand"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::CloseButtonCommandProperty(); } },
          { L"closebuttoncommandparameter"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::CloseButtonCommandParameterProperty(); } },
          { L"primarybuttonstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::PrimaryButtonStyleProperty(); } },
          { L"secondarybuttonstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::SecondaryButtonStyleProperty(); } },
          { L"closebuttonstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::CloseButtonStyleProperty(); } },
          { L"defaultbutton"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentDialog::DefaultButtonProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ContentPresenter::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ContentPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::ContentProperty(); } },
          { L"contenttemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::ContentTemplateProperty(); } },
          { L"contenttemplateselector"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::ContentTemplateSelectorProperty(); } },
          { L"contenttransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::ContentTransitionsProperty(); } },
          { L"fontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::FontStretchProperty(); } },
          { L"characterspacing"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::CharacterSpacingProperty(); } },
          { L"foreground"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::ForegroundProperty(); } },
          { L"opticalmarginalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::OpticalMarginAlignmentProperty(); } },
          { L"textlinebounds"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::TextLineBoundsProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::IsTextScaleFactorEnabledProperty(); } },
          { L"textwrapping"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::TextWrappingProperty(); } },
          { L"maxlines"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::MaxLinesProperty(); } },
          { L"linestackingstrategy"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::LineStackingStrategyProperty(); } },
          { L"lineheight"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::LineHeightProperty(); } },
          { L"borderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::BorderThicknessProperty(); } },
          { L"cornerradius"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::PaddingProperty(); } },
          { L"background"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::BackgroundProperty(); } },
          { L"horizontalcontentalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::HorizontalContentAlignmentProperty(); } },
          { L"verticalcontentalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::VerticalContentAlignmentProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Windows::UI::Xaml::Controls::ContentPresenter::BackgroundSizingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Control::FontSizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Control>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::FontStretchProperty(); } },
          { L"characterspacing"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::CharacterSpacingProperty(); } },
          { L"foreground"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::ForegroundProperty(); } },
          { L"istabstop"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::IsTabStopProperty(); } },
          { L"isenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::IsEnabledProperty(); } },
          { L"tabindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::TabIndexProperty(); } },
          { L"tabnavigation"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::TabNavigationProperty(); } },
          { L"template"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::TemplateProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::PaddingProperty(); } },
          { L"horizontalcontentalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::HorizontalContentAlignmentProperty(); } },
          { L"verticalcontentalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::VerticalContentAlignmentProperty(); } },
          { L"background"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::BackgroundProperty(); } },
          { L"borderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::BorderThicknessProperty(); } },
          { L"borderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::BorderBrushProperty(); } },
          { L"defaultstylekey"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::DefaultStyleKeyProperty(); } },
          { L"focusstate"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::FocusStateProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::IsTextScaleFactorEnabledProperty(); } },
          { L"usesystemfocusvisuals"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::UseSystemFocusVisualsProperty(); } },
          { L"istemplatefocustarget"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::IsTemplateFocusTargetProperty(); } },
          { L"isfocusengagementenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::IsFocusEngagementEnabledProperty(); } },
          { L"isfocusengaged"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::IsFocusEngagedProperty(); } },
          { L"requirespointer"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::RequiresPointerProperty(); } },
          { L"xyfocusleft"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::XYFocusLeftProperty(); } },
          { L"xyfocusright"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::XYFocusRightProperty(); } },
          { L"xyfocusup"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::XYFocusUpProperty(); } },
          { L"xyfocusdown"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::XYFocusDownProperty(); } },
          { L"elementsoundmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::ElementSoundModeProperty(); } },
          { L"defaultstyleresourceuri"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::DefaultStyleResourceUriProperty(); } },
          { L"istemplatekeytiptarget"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::IsTemplateKeyTipTargetProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::BackgroundSizingProperty(); } },
          { L"cornerradius"s, []() { return winrt::Windows::UI::Xaml::Controls::Control::CornerRadiusProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::DatePicker::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::DatePicker>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::HeaderTemplateProperty(); } },
          { L"calendaridentifier"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::CalendarIdentifierProperty(); } },
          { L"date"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::DateProperty(); } },
          { L"dayvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::DayVisibleProperty(); } },
          { L"monthvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::MonthVisibleProperty(); } },
          { L"yearvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::YearVisibleProperty(); } },
          { L"dayformat"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::DayFormatProperty(); } },
          { L"monthformat"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::MonthFormatProperty(); } },
          { L"yearformat"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::YearFormatProperty(); } },
          { L"minyear"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::MinYearProperty(); } },
          { L"maxyear"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::MaxYearProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::OrientationProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::LightDismissOverlayModeProperty(); } },
          { L"selecteddate"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePicker::SelectedDateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::CalendarIdentifierProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::DatePickerFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"calendaridentifier"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::CalendarIdentifierProperty(); } },
          { L"date"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::DateProperty(); } },
          { L"dayvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::DayVisibleProperty(); } },
          { L"monthvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::MonthVisibleProperty(); } },
          { L"yearvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::YearVisibleProperty(); } },
          { L"minyear"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::MinYearProperty(); } },
          { L"maxyear"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::MaxYearProperty(); } },
          { L"dayformat"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::DayFormatProperty(); } },
          { L"monthformat"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::MonthFormatProperty(); } },
          { L"yearformat"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyout::YearFormatProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::DatePickerFlyoutItem::PrimaryTextProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::DatePickerFlyoutItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"primarytext"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyoutItem::PrimaryTextProperty(); } },
          { L"secondarytext"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyoutItem::SecondaryTextProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::DatePickerFlyoutPresenter::IsDefaultShadowEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::DatePickerFlyoutPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdefaultshadowenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::DatePickerFlyoutPresenter::IsDefaultShadowEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::FlipView::UseTouchAnimationsForAllNavigationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::FlipView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"usetouchanimationsforallnavigation"s, []() { return winrt::Windows::UI::Xaml::Controls::FlipView::UseTouchAnimationsForAllNavigationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Flyout::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Flyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Windows::UI::Xaml::Controls::Flyout::ContentProperty(); } },
          { L"flyoutpresenterstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::Flyout::FlyoutPresenterStyleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::FlyoutPresenter::IsDefaultShadowEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::FlyoutPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdefaultshadowenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::FlyoutPresenter::IsDefaultShadowEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::FontIcon::GlyphProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::FontIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"glyph"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIcon::GlyphProperty(); } },
          { L"fontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIcon::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIcon::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIcon::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIcon::FontStyleProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIcon::IsTextScaleFactorEnabledProperty(); } },
          { L"mirroredwhenrighttoleft"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIcon::MirroredWhenRightToLeftProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::FontIconSource::GlyphProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::FontIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"glyph"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIconSource::GlyphProperty(); } },
          { L"fontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIconSource::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIconSource::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIconSource::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIconSource::FontStyleProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIconSource::IsTextScaleFactorEnabledProperty(); } },
          { L"mirroredwhenrighttoleft"s, []() { return winrt::Windows::UI::Xaml::Controls::FontIconSource::MirroredWhenRightToLeftProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Frame::CacheSizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Frame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"cachesize"s, []() { return winrt::Windows::UI::Xaml::Controls::Frame::CacheSizeProperty(); } },
          { L"cangoback"s, []() { return winrt::Windows::UI::Xaml::Controls::Frame::CanGoBackProperty(); } },
          { L"cangoforward"s, []() { return winrt::Windows::UI::Xaml::Controls::Frame::CanGoForwardProperty(); } },
          { L"currentsourcepagetype"s, []() { return winrt::Windows::UI::Xaml::Controls::Frame::CurrentSourcePageTypeProperty(); } },
          { L"sourcepagetype"s, []() { return winrt::Windows::UI::Xaml::Controls::Frame::SourcePageTypeProperty(); } },
          { L"backstackdepth"s, []() { return winrt::Windows::UI::Xaml::Controls::Frame::BackStackDepthProperty(); } },
          { L"backstack"s, []() { return winrt::Windows::UI::Xaml::Controls::Frame::BackStackProperty(); } },
          { L"forwardstack"s, []() { return winrt::Windows::UI::Xaml::Controls::Frame::ForwardStackProperty(); } },
          { L"isnavigationstackenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Frame::IsNavigationStackEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Grid::RowProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Grid>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"row"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::RowProperty(); } },
          { L"column"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::ColumnProperty(); } },
          { L"rowspan"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::RowSpanProperty(); } },
          { L"columnspan"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::ColumnSpanProperty(); } },
          { L"borderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::BorderThicknessProperty(); } },
          { L"cornerradius"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::PaddingProperty(); } },
          { L"rowspacing"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::RowSpacingProperty(); } },
          { L"columnspacing"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::ColumnSpacingProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Windows::UI::Xaml::Controls::Grid::BackgroundSizingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::HandwritingView::PlacementTargetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::HandwritingView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"placementtarget"s, []() { return winrt::Windows::UI::Xaml::Controls::HandwritingView::PlacementTargetProperty(); } },
          { L"placementalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::HandwritingView::PlacementAlignmentProperty(); } },
          { L"isopen"s, []() { return winrt::Windows::UI::Xaml::Controls::HandwritingView::IsOpenProperty(); } },
          { L"arecandidatesenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::HandwritingView::AreCandidatesEnabledProperty(); } },
          { L"isswitchtokeyboardenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::HandwritingView::IsSwitchToKeyboardEnabledProperty(); } },
          { L"iscommandbaropen"s, []() { return winrt::Windows::UI::Xaml::Controls::HandwritingView::IsCommandBarOpenProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Hub::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Hub>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::Hub::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::Hub::HeaderTemplateProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::Hub::OrientationProperty(); } },
          { L"defaultsectionindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Hub::DefaultSectionIndexProperty(); } },
          { L"semanticzoomowner"s, []() { return winrt::Windows::UI::Xaml::Controls::Hub::SemanticZoomOwnerProperty(); } },
          { L"isactiveview"s, []() { return winrt::Windows::UI::Xaml::Controls::Hub::IsActiveViewProperty(); } },
          { L"iszoomedinview"s, []() { return winrt::Windows::UI::Xaml::Controls::Hub::IsZoomedInViewProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::HubSection::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::HubSection>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::HubSection::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::HubSection::HeaderTemplateProperty(); } },
          { L"contenttemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::HubSection::ContentTemplateProperty(); } },
          { L"isheaderinteractive"s, []() { return winrt::Windows::UI::Xaml::Controls::HubSection::IsHeaderInteractiveProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::HyperlinkButton::NavigateUriProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::HyperlinkButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"navigateuri"s, []() { return winrt::Windows::UI::Xaml::Controls::HyperlinkButton::NavigateUriProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::IconElement::ForegroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::IconElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"foreground"s, []() { return winrt::Windows::UI::Xaml::Controls::IconElement::ForegroundProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::IconSource::ForegroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::IconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"foreground"s, []() { return winrt::Windows::UI::Xaml::Controls::IconSource::ForegroundProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::IconSourceElement::IconSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::IconSourceElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iconsource"s, []() { return winrt::Windows::UI::Xaml::Controls::IconSourceElement::IconSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Image::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Image>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Windows::UI::Xaml::Controls::Image::SourceProperty(); } },
          { L"stretch"s, []() { return winrt::Windows::UI::Xaml::Controls::Image::StretchProperty(); } },
          { L"ninegrid"s, []() { return winrt::Windows::UI::Xaml::Controls::Image::NineGridProperty(); } },
          { L"playtosource"s, []() { return winrt::Windows::UI::Xaml::Controls::Image::PlayToSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbar::InitialControlsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"initialcontrols"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::InitialControlsProperty(); } },
          { L"children"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::ChildrenProperty(); } },
          { L"activetool"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::ActiveToolProperty(); } },
          { L"inkdrawingattributes"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::InkDrawingAttributesProperty(); } },
          { L"isrulerbuttonchecked"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::IsRulerButtonCheckedProperty(); } },
          { L"targetinkcanvas"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::TargetInkCanvasProperty(); } },
          { L"isstencilbuttonchecked"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::IsStencilButtonCheckedProperty(); } },
          { L"buttonflyoutplacement"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::ButtonFlyoutPlacementProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::OrientationProperty(); } },
          { L"targetinkpresenter"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbar::TargetInkPresenterProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarCustomPenButton::CustomPenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarCustomPenButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"custompen"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarCustomPenButton::CustomPenProperty(); } },
          { L"configurationcontent"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarCustomPenButton::ConfigurationContentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarCustomToolButton::ConfigurationContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarCustomToolButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"configurationcontent"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarCustomToolButton::ConfigurationContentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarEraserButton::IsClearAllVisibleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarEraserButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isclearallvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarEraserButton::IsClearAllVisibleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarFlyoutItem::KindProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarFlyoutItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"kind"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarFlyoutItem::KindProperty(); } },
          { L"ischecked"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarFlyoutItem::IsCheckedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarMenuButton::IsExtensionGlyphShownProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarMenuButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isextensionglyphshown"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarMenuButton::IsExtensionGlyphShownProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarPenButton::PaletteProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarPenButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"palette"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarPenButton::PaletteProperty(); } },
          { L"minstrokewidth"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarPenButton::MinStrokeWidthProperty(); } },
          { L"maxstrokewidth"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarPenButton::MaxStrokeWidthProperty(); } },
          { L"selectedbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarPenButton::SelectedBrushProperty(); } },
          { L"selectedbrushindex"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarPenButton::SelectedBrushIndexProperty(); } },
          { L"selectedstrokewidth"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarPenButton::SelectedStrokeWidthProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarPenConfigurationControl::PenButtonProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarPenConfigurationControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"penbutton"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarPenConfigurationControl::PenButtonProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarRulerButton::RulerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarRulerButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ruler"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarRulerButton::RulerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarStencilButton::RulerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarStencilButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ruler"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarStencilButton::RulerProperty(); } },
          { L"protractor"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarStencilButton::ProtractorProperty(); } },
          { L"selectedstencil"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarStencilButton::SelectedStencilProperty(); } },
          { L"isruleritemvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarStencilButton::IsRulerItemVisibleProperty(); } },
          { L"isprotractoritemvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarStencilButton::IsProtractorItemVisibleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::InkToolbarToolButton::IsExtensionGlyphShownProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::InkToolbarToolButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isextensionglyphshown"s, []() { return winrt::Windows::UI::Xaml::Controls::InkToolbarToolButton::IsExtensionGlyphShownProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ItemsControl::ItemsSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ItemsControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemssource"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::ItemsSourceProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::ItemTemplateProperty(); } },
          { L"itemtemplateselector"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::ItemTemplateSelectorProperty(); } },
          { L"itemspanel"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::ItemsPanelProperty(); } },
          { L"displaymemberpath"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::DisplayMemberPathProperty(); } },
          { L"itemcontainerstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::ItemContainerStyleProperty(); } },
          { L"itemcontainerstyleselector"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::ItemContainerStyleSelectorProperty(); } },
          { L"itemcontainertransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::ItemContainerTransitionsProperty(); } },
          { L"groupstyleselector"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::GroupStyleSelectorProperty(); } },
          { L"isgrouping"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsControl::IsGroupingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ItemsPresenter::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ItemsPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsPresenter::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsPresenter::HeaderTemplateProperty(); } },
          { L"headertransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsPresenter::HeaderTransitionsProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsPresenter::PaddingProperty(); } },
          { L"footer"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsPresenter::FooterProperty(); } },
          { L"footertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsPresenter::FooterTemplateProperty(); } },
          { L"footertransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsPresenter::FooterTransitionsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ItemsStackPanel::GroupPaddingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ItemsStackPanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"grouppadding"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsStackPanel::GroupPaddingProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsStackPanel::OrientationProperty(); } },
          { L"groupheaderplacement"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsStackPanel::GroupHeaderPlacementProperty(); } },
          { L"cachelength"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsStackPanel::CacheLengthProperty(); } },
          { L"arestickygroupheadersenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsStackPanel::AreStickyGroupHeadersEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid::GroupPaddingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"grouppadding"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid::GroupPaddingProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid::OrientationProperty(); } },
          { L"maximumrowsorcolumns"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid::MaximumRowsOrColumnsProperty(); } },
          { L"itemwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid::ItemWidthProperty(); } },
          { L"itemheight"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid::ItemHeightProperty(); } },
          { L"groupheaderplacement"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid::GroupHeaderPlacementProperty(); } },
          { L"cachelength"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid::CacheLengthProperty(); } },
          { L"arestickygroupheadersenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ItemsWrapGrid::AreStickyGroupHeadersEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ListBox::SelectionModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ListBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectionmode"s, []() { return winrt::Windows::UI::Xaml::Controls::ListBox::SelectionModeProperty(); } },
          { L"singleselectionfollowsfocus"s, []() { return winrt::Windows::UI::Xaml::Controls::ListBox::SingleSelectionFollowsFocusProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ListPickerFlyout::ItemsSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ListPickerFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemssource"s, []() { return winrt::Windows::UI::Xaml::Controls::ListPickerFlyout::ItemsSourceProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ListPickerFlyout::ItemTemplateProperty(); } },
          { L"displaymemberpath"s, []() { return winrt::Windows::UI::Xaml::Controls::ListPickerFlyout::DisplayMemberPathProperty(); } },
          { L"selectionmode"s, []() { return winrt::Windows::UI::Xaml::Controls::ListPickerFlyout::SelectionModeProperty(); } },
          { L"selectedindex"s, []() { return winrt::Windows::UI::Xaml::Controls::ListPickerFlyout::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Windows::UI::Xaml::Controls::ListPickerFlyout::SelectedItemProperty(); } },
          { L"selectedvalue"s, []() { return winrt::Windows::UI::Xaml::Controls::ListPickerFlyout::SelectedValueProperty(); } },
          { L"selectedvaluepath"s, []() { return winrt::Windows::UI::Xaml::Controls::ListPickerFlyout::SelectedValuePathProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ListViewBase::SelectionModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ListViewBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectionmode"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::SelectionModeProperty(); } },
          { L"isswipeenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::IsSwipeEnabledProperty(); } },
          { L"candragitems"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::CanDragItemsProperty(); } },
          { L"canreorderitems"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::CanReorderItemsProperty(); } },
          { L"isitemclickenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::IsItemClickEnabledProperty(); } },
          { L"datafetchsize"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::DataFetchSizeProperty(); } },
          { L"incrementalloadingthreshold"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::IncrementalLoadingThresholdProperty(); } },
          { L"incrementalloadingtrigger"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::IncrementalLoadingTriggerProperty(); } },
          { L"semanticzoomowner"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::SemanticZoomOwnerProperty(); } },
          { L"isactiveview"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::IsActiveViewProperty(); } },
          { L"iszoomedinview"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::IsZoomedInViewProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::HeaderTemplateProperty(); } },
          { L"headertransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::HeaderTransitionsProperty(); } },
          { L"showsscrollingplaceholders"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::ShowsScrollingPlaceholdersProperty(); } },
          { L"footer"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::FooterProperty(); } },
          { L"footertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::FooterTemplateProperty(); } },
          { L"footertransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::FooterTransitionsProperty(); } },
          { L"reordermode"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::ReorderModeProperty(); } },
          { L"ismultiselectcheckboxenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::IsMultiSelectCheckBoxEnabledProperty(); } },
          { L"singleselectionfollowsfocus"s, []() { return winrt::Windows::UI::Xaml::Controls::ListViewBase::SingleSelectionFollowsFocusProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MediaElement::PosterSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MediaElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"postersource"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::PosterSourceProperty(); } },
          { L"source"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::SourceProperty(); } },
          { L"ismuted"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::IsMutedProperty(); } },
          { L"isaudioonly"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::IsAudioOnlyProperty(); } },
          { L"autoplay"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::AutoPlayProperty(); } },
          { L"volume"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::VolumeProperty(); } },
          { L"balance"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::BalanceProperty(); } },
          { L"naturalvideoheight"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::NaturalVideoHeightProperty(); } },
          { L"naturalvideowidth"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::NaturalVideoWidthProperty(); } },
          { L"naturalduration"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::NaturalDurationProperty(); } },
          { L"position"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::PositionProperty(); } },
          { L"downloadprogress"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::DownloadProgressProperty(); } },
          { L"bufferingprogress"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::BufferingProgressProperty(); } },
          { L"downloadprogressoffset"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::DownloadProgressOffsetProperty(); } },
          { L"currentstate"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::CurrentStateProperty(); } },
          { L"canseek"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::CanSeekProperty(); } },
          { L"canpause"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::CanPauseProperty(); } },
          { L"audiostreamcount"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::AudioStreamCountProperty(); } },
          { L"audiostreamindex"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::AudioStreamIndexProperty(); } },
          { L"playbackrate"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::PlaybackRateProperty(); } },
          { L"islooping"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::IsLoopingProperty(); } },
          { L"playtosource"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::PlayToSourceProperty(); } },
          { L"defaultplaybackrate"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::DefaultPlaybackRateProperty(); } },
          { L"aspectratiowidth"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::AspectRatioWidthProperty(); } },
          { L"aspectratioheight"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::AspectRatioHeightProperty(); } },
          { L"realtimeplayback"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::RealTimePlaybackProperty(); } },
          { L"audiocategory"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::AudioCategoryProperty(); } },
          { L"audiodevicetype"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::AudioDeviceTypeProperty(); } },
          { L"protectionmanager"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::ProtectionManagerProperty(); } },
          { L"stereo3dvideopackingmode"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::Stereo3DVideoPackingModeProperty(); } },
          { L"stereo3dvideorendermode"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::Stereo3DVideoRenderModeProperty(); } },
          { L"isstereo3dvideo"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::IsStereo3DVideoProperty(); } },
          { L"actualstereo3dvideopackingmode"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::ActualStereo3DVideoPackingModeProperty(); } },
          { L"aretransportcontrolsenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::AreTransportControlsEnabledProperty(); } },
          { L"stretch"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::StretchProperty(); } },
          { L"isfullwindow"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::IsFullWindowProperty(); } },
          { L"playtopreferredsourceuri"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaElement::PlayToPreferredSourceUriProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MediaPlayerElement::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MediaPlayerElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerElement::SourceProperty(); } },
          { L"aretransportcontrolsenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerElement::AreTransportControlsEnabledProperty(); } },
          { L"postersource"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerElement::PosterSourceProperty(); } },
          { L"stretch"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerElement::StretchProperty(); } },
          { L"autoplay"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerElement::AutoPlayProperty(); } },
          { L"isfullwindow"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerElement::IsFullWindowProperty(); } },
          { L"mediaplayer"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerElement::MediaPlayerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MediaPlayerPresenter::MediaPlayerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MediaPlayerPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"mediaplayer"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerPresenter::MediaPlayerProperty(); } },
          { L"stretch"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerPresenter::StretchProperty(); } },
          { L"isfullwindow"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaPlayerPresenter::IsFullWindowProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsFullWindowButtonVisibleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MediaTransportControls>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isfullwindowbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsFullWindowButtonVisibleProperty(); } },
          { L"isfullwindowenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsFullWindowEnabledProperty(); } },
          { L"iszoombuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsZoomButtonVisibleProperty(); } },
          { L"iszoomenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsZoomEnabledProperty(); } },
          { L"isfastforwardbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsFastForwardButtonVisibleProperty(); } },
          { L"isfastforwardenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsFastForwardEnabledProperty(); } },
          { L"isfastrewindbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsFastRewindButtonVisibleProperty(); } },
          { L"isfastrewindenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsFastRewindEnabledProperty(); } },
          { L"isstopbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsStopButtonVisibleProperty(); } },
          { L"isstopenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsStopEnabledProperty(); } },
          { L"isvolumebuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsVolumeButtonVisibleProperty(); } },
          { L"isvolumeenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsVolumeEnabledProperty(); } },
          { L"isplaybackratebuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsPlaybackRateButtonVisibleProperty(); } },
          { L"isplaybackrateenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsPlaybackRateEnabledProperty(); } },
          { L"isseekbarvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsSeekBarVisibleProperty(); } },
          { L"isseekenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsSeekEnabledProperty(); } },
          { L"iscompact"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsCompactProperty(); } },
          { L"isskipforwardbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsSkipForwardButtonVisibleProperty(); } },
          { L"isskipforwardenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsSkipForwardEnabledProperty(); } },
          { L"isskipbackwardbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsSkipBackwardButtonVisibleProperty(); } },
          { L"isskipbackwardenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsSkipBackwardEnabledProperty(); } },
          { L"isnexttrackbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsNextTrackButtonVisibleProperty(); } },
          { L"isprevioustrackbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsPreviousTrackButtonVisibleProperty(); } },
          { L"fastplayfallbackbehaviour"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::FastPlayFallbackBehaviourProperty(); } },
          { L"showandhideautomatically"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::ShowAndHideAutomaticallyProperty(); } },
          { L"isrepeatenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsRepeatEnabledProperty(); } },
          { L"isrepeatbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsRepeatButtonVisibleProperty(); } },
          { L"iscompactoverlaybuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsCompactOverlayButtonVisibleProperty(); } },
          { L"iscompactoverlayenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControls::IsCompactOverlayEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MediaTransportControlsHelper::DropoutOrderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MediaTransportControlsHelper>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"dropoutorder"s, []() { return winrt::Windows::UI::Xaml::Controls::MediaTransportControlsHelper::DropoutOrderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MenuBar::ItemsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MenuBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"items"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuBar::ItemsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MenuBarItem::TitleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MenuBarItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"title"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuBarItem::TitleProperty(); } },
          { L"items"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuBarItem::ItemsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MenuFlyout::MenuFlyoutPresenterStyleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MenuFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"menuflyoutpresenterstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuFlyout::MenuFlyoutPresenterStyleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem::TextProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"text"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem::TextProperty(); } },
          { L"command"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem::CommandProperty(); } },
          { L"commandparameter"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem::CommandParameterProperty(); } },
          { L"icon"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem::IconProperty(); } },
          { L"keyboardacceleratortextoverride"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem::KeyboardAcceleratorTextOverrideProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MenuFlyoutPresenter::IsDefaultShadowEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MenuFlyoutPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdefaultshadowenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuFlyoutPresenter::IsDefaultShadowEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::MenuFlyoutSubItem::TextProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::MenuFlyoutSubItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"text"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuFlyoutSubItem::TextProperty(); } },
          { L"icon"s, []() { return winrt::Windows::UI::Xaml::Controls::MenuFlyoutSubItem::IconProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::NavigationView::IsPaneOpenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::NavigationView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ispaneopen"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::IsPaneOpenProperty(); } },
          { L"compactmodethresholdwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::CompactModeThresholdWidthProperty(); } },
          { L"expandedmodethresholdwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::ExpandedModeThresholdWidthProperty(); } },
          { L"panefooter"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::PaneFooterProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::HeaderTemplateProperty(); } },
          { L"displaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::DisplayModeProperty(); } },
          { L"issettingsvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::IsSettingsVisibleProperty(); } },
          { L"ispanetogglebuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::IsPaneToggleButtonVisibleProperty(); } },
          { L"alwaysshowheader"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::AlwaysShowHeaderProperty(); } },
          { L"compactpanelength"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::CompactPaneLengthProperty(); } },
          { L"openpanelength"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::OpenPaneLengthProperty(); } },
          { L"panetogglebuttonstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::PaneToggleButtonStyleProperty(); } },
          { L"menuitems"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::MenuItemsProperty(); } },
          { L"menuitemssource"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::MenuItemsSourceProperty(); } },
          { L"selecteditem"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::SelectedItemProperty(); } },
          { L"settingsitem"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::SettingsItemProperty(); } },
          { L"autosuggestbox"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::AutoSuggestBoxProperty(); } },
          { L"menuitemtemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::MenuItemTemplateProperty(); } },
          { L"menuitemtemplateselector"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::MenuItemTemplateSelectorProperty(); } },
          { L"menuitemcontainerstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::MenuItemContainerStyleProperty(); } },
          { L"menuitemcontainerstyleselector"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::MenuItemContainerStyleSelectorProperty(); } },
          { L"isbackbuttonvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::IsBackButtonVisibleProperty(); } },
          { L"isbackenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::IsBackEnabledProperty(); } },
          { L"panetitle"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::PaneTitleProperty(); } },
          { L"panedisplaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::PaneDisplayModeProperty(); } },
          { L"paneheader"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::PaneHeaderProperty(); } },
          { L"panecustomcontent"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::PaneCustomContentProperty(); } },
          { L"contentoverlay"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::ContentOverlayProperty(); } },
          { L"ispanevisible"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::IsPaneVisibleProperty(); } },
          { L"selectionfollowsfocus"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::SelectionFollowsFocusProperty(); } },
          { L"templatesettings"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::TemplateSettingsProperty(); } },
          { L"shouldernavigationenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::ShoulderNavigationEnabledProperty(); } },
          { L"overflowlabelmode"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationView::OverflowLabelModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::NavigationViewItem::IconProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::NavigationViewItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"icon"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewItem::IconProperty(); } },
          { L"compactpanelength"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewItem::CompactPaneLengthProperty(); } },
          { L"selectsoninvoked"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewItem::SelectsOnInvokedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::NavigationViewTemplateSettings::TopPaddingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::NavigationViewTemplateSettings>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"toppadding"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewTemplateSettings::TopPaddingProperty(); } },
          { L"overflowbuttonvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewTemplateSettings::OverflowButtonVisibilityProperty(); } },
          { L"panetogglebuttonvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewTemplateSettings::PaneToggleButtonVisibilityProperty(); } },
          { L"backbuttonvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewTemplateSettings::BackButtonVisibilityProperty(); } },
          { L"toppanevisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewTemplateSettings::TopPaneVisibilityProperty(); } },
          { L"leftpanevisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewTemplateSettings::LeftPaneVisibilityProperty(); } },
          { L"singleselectionfollowsfocus"s, []() { return winrt::Windows::UI::Xaml::Controls::NavigationViewTemplateSettings::SingleSelectionFollowsFocusProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Page::FrameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Page>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"frame"s, []() { return winrt::Windows::UI::Xaml::Controls::Page::FrameProperty(); } },
          { L"topappbar"s, []() { return winrt::Windows::UI::Xaml::Controls::Page::TopAppBarProperty(); } },
          { L"bottomappbar"s, []() { return winrt::Windows::UI::Xaml::Controls::Page::BottomAppBarProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Panel::BackgroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Panel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"background"s, []() { return winrt::Windows::UI::Xaml::Controls::Panel::BackgroundProperty(); } },
          { L"isitemshost"s, []() { return winrt::Windows::UI::Xaml::Controls::Panel::IsItemsHostProperty(); } },
          { L"childrentransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::Panel::ChildrenTransitionsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ParallaxView::ChildProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ParallaxView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"child"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::ChildProperty(); } },
          { L"horizontalsourceendoffset"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::HorizontalSourceEndOffsetProperty(); } },
          { L"horizontalsourceoffsetkind"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::HorizontalSourceOffsetKindProperty(); } },
          { L"horizontalsourcestartoffset"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::HorizontalSourceStartOffsetProperty(); } },
          { L"maxhorizontalshiftratio"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::MaxHorizontalShiftRatioProperty(); } },
          { L"horizontalshift"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::HorizontalShiftProperty(); } },
          { L"ishorizontalshiftclamped"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::IsHorizontalShiftClampedProperty(); } },
          { L"isverticalshiftclamped"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::IsVerticalShiftClampedProperty(); } },
          { L"source"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::SourceProperty(); } },
          { L"verticalsourceendoffset"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::VerticalSourceEndOffsetProperty(); } },
          { L"verticalsourceoffsetkind"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::VerticalSourceOffsetKindProperty(); } },
          { L"verticalsourcestartoffset"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::VerticalSourceStartOffsetProperty(); } },
          { L"maxverticalshiftratio"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::MaxVerticalShiftRatioProperty(); } },
          { L"verticalshift"s, []() { return winrt::Windows::UI::Xaml::Controls::ParallaxView::VerticalShiftProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::PasswordBox::PasswordProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::PasswordBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"password"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::PasswordProperty(); } },
          { L"passwordchar"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::PasswordCharProperty(); } },
          { L"ispasswordrevealbuttonenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::IsPasswordRevealButtonEnabledProperty(); } },
          { L"maxlength"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::MaxLengthProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::HeaderTemplateProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::PlaceholderTextProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::SelectionHighlightColorProperty(); } },
          { L"preventkeyboarddisplayonprogrammaticfocus"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::PreventKeyboardDisplayOnProgrammaticFocusProperty(); } },
          { L"passwordrevealmode"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::PasswordRevealModeProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::TextReadingOrderProperty(); } },
          { L"inputscope"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::InputScopeProperty(); } },
          { L"canpasteclipboardcontent"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::CanPasteClipboardContentProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::SelectionFlyoutProperty(); } },
          { L"description"s, []() { return winrt::Windows::UI::Xaml::Controls::PasswordBox::DescriptionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::PathIcon::DataProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::PathIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"data"s, []() { return winrt::Windows::UI::Xaml::Controls::PathIcon::DataProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::PathIconSource::DataProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::PathIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"data"s, []() { return winrt::Windows::UI::Xaml::Controls::PathIconSource::DataProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::PersonPicture::BadgeNumberProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::PersonPicture>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"badgenumber"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::BadgeNumberProperty(); } },
          { L"badgeglyph"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::BadgeGlyphProperty(); } },
          { L"badgeimagesource"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::BadgeImageSourceProperty(); } },
          { L"badgetext"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::BadgeTextProperty(); } },
          { L"isgroup"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::IsGroupProperty(); } },
          { L"contact"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::ContactProperty(); } },
          { L"displayname"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::DisplayNameProperty(); } },
          { L"initials"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::InitialsProperty(); } },
          { L"prefersmallimage"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::PreferSmallImageProperty(); } },
          { L"profilepicture"s, []() { return winrt::Windows::UI::Xaml::Controls::PersonPicture::ProfilePictureProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::PickerFlyout::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::PickerFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Windows::UI::Xaml::Controls::PickerFlyout::ContentProperty(); } },
          { L"confirmationbuttonsvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::PickerFlyout::ConfirmationButtonsVisibleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Pivot::TitleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Pivot>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"title"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::TitleProperty(); } },
          { L"titletemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::TitleTemplateProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::HeaderTemplateProperty(); } },
          { L"selectedindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::SelectedItemProperty(); } },
          { L"islocked"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::IsLockedProperty(); } },
          { L"slideinanimationgroup"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::SlideInAnimationGroupProperty(); } },
          { L"leftheader"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::LeftHeaderProperty(); } },
          { L"leftheadertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::LeftHeaderTemplateProperty(); } },
          { L"rightheader"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::RightHeaderProperty(); } },
          { L"rightheadertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::RightHeaderTemplateProperty(); } },
          { L"headerfocusvisualplacement"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::HeaderFocusVisualPlacementProperty(); } },
          { L"isheaderitemscarouselenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Pivot::IsHeaderItemsCarouselEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::PivotItem::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::PivotItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::PivotItem::HeaderProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ProgressBar::IsIndeterminateProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ProgressBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isindeterminate"s, []() { return winrt::Windows::UI::Xaml::Controls::ProgressBar::IsIndeterminateProperty(); } },
          { L"showerror"s, []() { return winrt::Windows::UI::Xaml::Controls::ProgressBar::ShowErrorProperty(); } },
          { L"showpaused"s, []() { return winrt::Windows::UI::Xaml::Controls::ProgressBar::ShowPausedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ProgressRing::IsActiveProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ProgressRing>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isactive"s, []() { return winrt::Windows::UI::Xaml::Controls::ProgressRing::IsActiveProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RadioButton::GroupNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RadioButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"groupname"s, []() { return winrt::Windows::UI::Xaml::Controls::RadioButton::GroupNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RatingControl::CaptionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RatingControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"caption"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingControl::CaptionProperty(); } },
          { L"initialsetvalue"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingControl::InitialSetValueProperty(); } },
          { L"isclearenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingControl::IsClearEnabledProperty(); } },
          { L"isreadonly"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingControl::IsReadOnlyProperty(); } },
          { L"maxrating"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingControl::MaxRatingProperty(); } },
          { L"placeholdervalue"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingControl::PlaceholderValueProperty(); } },
          { L"iteminfo"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingControl::ItemInfoProperty(); } },
          { L"value"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingControl::ValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RatingItemFontInfo::DisabledGlyphProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RatingItemFontInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"disabledglyph"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemFontInfo::DisabledGlyphProperty(); } },
          { L"glyph"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemFontInfo::GlyphProperty(); } },
          { L"placeholderglyph"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemFontInfo::PlaceholderGlyphProperty(); } },
          { L"pointeroverglyph"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemFontInfo::PointerOverGlyphProperty(); } },
          { L"pointeroverplaceholderglyph"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemFontInfo::PointerOverPlaceholderGlyphProperty(); } },
          { L"unsetglyph"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemFontInfo::UnsetGlyphProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RatingItemImageInfo::DisabledImageProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RatingItemImageInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"disabledimage"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemImageInfo::DisabledImageProperty(); } },
          { L"image"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemImageInfo::ImageProperty(); } },
          { L"placeholderimage"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemImageInfo::PlaceholderImageProperty(); } },
          { L"pointeroverimage"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemImageInfo::PointerOverImageProperty(); } },
          { L"pointeroverplaceholderimage"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemImageInfo::PointerOverPlaceholderImageProperty(); } },
          { L"unsetimage"s, []() { return winrt::Windows::UI::Xaml::Controls::RatingItemImageInfo::UnsetImageProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RefreshContainer::VisualizerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RefreshContainer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"visualizer"s, []() { return winrt::Windows::UI::Xaml::Controls::RefreshContainer::VisualizerProperty(); } },
          { L"pulldirection"s, []() { return winrt::Windows::UI::Xaml::Controls::RefreshContainer::PullDirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RefreshVisualizer::InfoProviderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RefreshVisualizer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"infoprovider"s, []() { return winrt::Windows::UI::Xaml::Controls::RefreshVisualizer::InfoProviderProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::RefreshVisualizer::OrientationProperty(); } },
          { L"content"s, []() { return winrt::Windows::UI::Xaml::Controls::RefreshVisualizer::ContentProperty(); } },
          { L"state"s, []() { return winrt::Windows::UI::Xaml::Controls::RefreshVisualizer::StateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RelativePanel::LeftOfProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RelativePanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"leftof"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::LeftOfProperty(); } },
          { L"above"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AboveProperty(); } },
          { L"rightof"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::RightOfProperty(); } },
          { L"below"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::BelowProperty(); } },
          { L"alignhorizontalcenterwith"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignHorizontalCenterWithProperty(); } },
          { L"alignverticalcenterwith"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignVerticalCenterWithProperty(); } },
          { L"alignleftwith"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignLeftWithProperty(); } },
          { L"aligntopwith"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignTopWithProperty(); } },
          { L"alignrightwith"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignRightWithProperty(); } },
          { L"alignbottomwith"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignBottomWithProperty(); } },
          { L"alignleftwithpanel"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignLeftWithPanelProperty(); } },
          { L"aligntopwithpanel"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignTopWithPanelProperty(); } },
          { L"alignrightwithpanel"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignRightWithPanelProperty(); } },
          { L"alignbottomwithpanel"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignBottomWithPanelProperty(); } },
          { L"alignhorizontalcenterwithpanel"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignHorizontalCenterWithPanelProperty(); } },
          { L"alignverticalcenterwithpanel"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::AlignVerticalCenterWithPanelProperty(); } },
          { L"borderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::BorderThicknessProperty(); } },
          { L"cornerradius"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::PaddingProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Windows::UI::Xaml::Controls::RelativePanel::BackgroundSizingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RichEditBox::IsReadOnlyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RichEditBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isreadonly"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::IsReadOnlyProperty(); } },
          { L"acceptsreturn"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::AcceptsReturnProperty(); } },
          { L"textalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::TextAlignmentProperty(); } },
          { L"textwrapping"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::TextWrappingProperty(); } },
          { L"isspellcheckenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::IsSpellCheckEnabledProperty(); } },
          { L"istextpredictionenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::IsTextPredictionEnabledProperty(); } },
          { L"inputscope"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::InputScopeProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::HeaderTemplateProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::PlaceholderTextProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::SelectionHighlightColorProperty(); } },
          { L"preventkeyboarddisplayonprogrammaticfocus"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::PreventKeyboardDisplayOnProgrammaticFocusProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::IsColorFontEnabledProperty(); } },
          { L"desiredcandidatewindowalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::DesiredCandidateWindowAlignmentProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::TextReadingOrderProperty(); } },
          { L"clipboardcopyformat"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::ClipboardCopyFormatProperty(); } },
          { L"selectionhighlightcolorwhennotfocused"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::SelectionHighlightColorWhenNotFocusedProperty(); } },
          { L"maxlength"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::MaxLengthProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::HorizontalTextAlignmentProperty(); } },
          { L"charactercasing"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::CharacterCasingProperty(); } },
          { L"disabledformattingaccelerators"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::DisabledFormattingAcceleratorsProperty(); } },
          { L"contentlinkforegroundcolor"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::ContentLinkForegroundColorProperty(); } },
          { L"contentlinkbackgroundcolor"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::ContentLinkBackgroundColorProperty(); } },
          { L"contentlinkproviders"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::ContentLinkProvidersProperty(); } },
          { L"handwritingview"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::HandwritingViewProperty(); } },
          { L"ishandwritingviewenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::IsHandwritingViewEnabledProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::SelectionFlyoutProperty(); } },
          { L"proofingmenuflyout"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::ProofingMenuFlyoutProperty(); } },
          { L"description"s, []() { return winrt::Windows::UI::Xaml::Controls::RichEditBox::DescriptionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RichTextBlock::FontSizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RichTextBlock>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::FontStretchProperty(); } },
          { L"foreground"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::ForegroundProperty(); } },
          { L"textwrapping"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::TextWrappingProperty(); } },
          { L"texttrimming"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::TextTrimmingProperty(); } },
          { L"textalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::TextAlignmentProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::PaddingProperty(); } },
          { L"lineheight"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::LineHeightProperty(); } },
          { L"linestackingstrategy"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::LineStackingStrategyProperty(); } },
          { L"characterspacing"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::CharacterSpacingProperty(); } },
          { L"overflowcontenttarget"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::OverflowContentTargetProperty(); } },
          { L"istextselectionenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::IsTextSelectionEnabledProperty(); } },
          { L"hasoverflowcontent"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::HasOverflowContentProperty(); } },
          { L"selectedtext"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::SelectedTextProperty(); } },
          { L"textindent"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::TextIndentProperty(); } },
          { L"maxlines"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::MaxLinesProperty(); } },
          { L"textlinebounds"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::TextLineBoundsProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::SelectionHighlightColorProperty(); } },
          { L"opticalmarginalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::OpticalMarginAlignmentProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::IsColorFontEnabledProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::TextReadingOrderProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::IsTextScaleFactorEnabledProperty(); } },
          { L"textdecorations"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::TextDecorationsProperty(); } },
          { L"istexttrimmed"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::IsTextTrimmedProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::HorizontalTextAlignmentProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlock::SelectionFlyoutProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RichTextBlockOverflow::OverflowContentTargetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RichTextBlockOverflow>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"overflowcontenttarget"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlockOverflow::OverflowContentTargetProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlockOverflow::PaddingProperty(); } },
          { L"hasoverflowcontent"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlockOverflow::HasOverflowContentProperty(); } },
          { L"maxlines"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlockOverflow::MaxLinesProperty(); } },
          { L"istexttrimmed"s, []() { return winrt::Windows::UI::Xaml::Controls::RichTextBlockOverflow::IsTextTrimmedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::RowDefinition::HeightProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::RowDefinition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"height"s, []() { return winrt::Windows::UI::Xaml::Controls::RowDefinition::HeightProperty(); } },
          { L"maxheight"s, []() { return winrt::Windows::UI::Xaml::Controls::RowDefinition::MaxHeightProperty(); } },
          { L"minheight"s, []() { return winrt::Windows::UI::Xaml::Controls::RowDefinition::MinHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ScrollContentPresenter::CanContentRenderOutsideBoundsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ScrollContentPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"cancontentrenderoutsidebounds"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollContentPresenter::CanContentRenderOutsideBoundsProperty(); } },
          { L"sizescontenttotemplatedparent"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollContentPresenter::SizesContentToTemplatedParentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ScrollViewer::HorizontalSnapPointsAlignmentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ScrollViewer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"horizontalsnappointsalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::HorizontalSnapPointsAlignmentProperty(); } },
          { L"verticalsnappointsalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::VerticalSnapPointsAlignmentProperty(); } },
          { L"horizontalsnappointstype"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::HorizontalSnapPointsTypeProperty(); } },
          { L"verticalsnappointstype"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::VerticalSnapPointsTypeProperty(); } },
          { L"zoomsnappointstype"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ZoomSnapPointsTypeProperty(); } },
          { L"horizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::HorizontalOffsetProperty(); } },
          { L"viewportwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ViewportWidthProperty(); } },
          { L"scrollablewidth"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ScrollableWidthProperty(); } },
          { L"computedhorizontalscrollbarvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ComputedHorizontalScrollBarVisibilityProperty(); } },
          { L"extentwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ExtentWidthProperty(); } },
          { L"verticaloffset"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::VerticalOffsetProperty(); } },
          { L"viewportheight"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ViewportHeightProperty(); } },
          { L"scrollableheight"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ScrollableHeightProperty(); } },
          { L"computedverticalscrollbarvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ComputedVerticalScrollBarVisibilityProperty(); } },
          { L"extentheight"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ExtentHeightProperty(); } },
          { L"minzoomfactor"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::MinZoomFactorProperty(); } },
          { L"maxzoomfactor"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::MaxZoomFactorProperty(); } },
          { L"zoomfactor"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ZoomFactorProperty(); } },
          { L"zoomsnappoints"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ZoomSnapPointsProperty(); } },
          { L"horizontalscrollbarvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::HorizontalScrollBarVisibilityProperty(); } },
          { L"verticalscrollbarvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::VerticalScrollBarVisibilityProperty(); } },
          { L"ishorizontalrailenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::IsHorizontalRailEnabledProperty(); } },
          { L"isverticalrailenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::IsVerticalRailEnabledProperty(); } },
          { L"ishorizontalscrollchainingenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::IsHorizontalScrollChainingEnabledProperty(); } },
          { L"isverticalscrollchainingenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::IsVerticalScrollChainingEnabledProperty(); } },
          { L"iszoomchainingenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::IsZoomChainingEnabledProperty(); } },
          { L"isscrollinertiaenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::IsScrollInertiaEnabledProperty(); } },
          { L"iszoominertiaenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::IsZoomInertiaEnabledProperty(); } },
          { L"horizontalscrollmode"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::HorizontalScrollModeProperty(); } },
          { L"verticalscrollmode"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::VerticalScrollModeProperty(); } },
          { L"zoommode"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ZoomModeProperty(); } },
          { L"isdeferredscrollingenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::IsDeferredScrollingEnabledProperty(); } },
          { L"bringintoviewonfocuschange"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::BringIntoViewOnFocusChangeProperty(); } },
          { L"topleftheader"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::TopLeftHeaderProperty(); } },
          { L"leftheader"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::LeftHeaderProperty(); } },
          { L"topheader"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::TopHeaderProperty(); } },
          { L"reduceviewportforcoreinputviewocclusions"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::ReduceViewportForCoreInputViewOcclusionsProperty(); } },
          { L"horizontalanchorratio"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::HorizontalAnchorRatioProperty(); } },
          { L"verticalanchorratio"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::VerticalAnchorRatioProperty(); } },
          { L"cancontentrenderoutsidebounds"s, []() { return winrt::Windows::UI::Xaml::Controls::ScrollViewer::CanContentRenderOutsideBoundsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SearchBox::SearchHistoryEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SearchBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"searchhistoryenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::SearchBox::SearchHistoryEnabledProperty(); } },
          { L"searchhistorycontext"s, []() { return winrt::Windows::UI::Xaml::Controls::SearchBox::SearchHistoryContextProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Windows::UI::Xaml::Controls::SearchBox::PlaceholderTextProperty(); } },
          { L"querytext"s, []() { return winrt::Windows::UI::Xaml::Controls::SearchBox::QueryTextProperty(); } },
          { L"focusonkeyboardinput"s, []() { return winrt::Windows::UI::Xaml::Controls::SearchBox::FocusOnKeyboardInputProperty(); } },
          { L"choosesuggestiononenter"s, []() { return winrt::Windows::UI::Xaml::Controls::SearchBox::ChooseSuggestionOnEnterProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SemanticZoom::ZoomedInViewProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SemanticZoom>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"zoomedinview"s, []() { return winrt::Windows::UI::Xaml::Controls::SemanticZoom::ZoomedInViewProperty(); } },
          { L"zoomedoutview"s, []() { return winrt::Windows::UI::Xaml::Controls::SemanticZoom::ZoomedOutViewProperty(); } },
          { L"iszoomedinviewactive"s, []() { return winrt::Windows::UI::Xaml::Controls::SemanticZoom::IsZoomedInViewActiveProperty(); } },
          { L"canchangeviews"s, []() { return winrt::Windows::UI::Xaml::Controls::SemanticZoom::CanChangeViewsProperty(); } },
          { L"iszoomoutbuttonenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::SemanticZoom::IsZoomOutButtonEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SettingsFlyout::TitleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SettingsFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"title"s, []() { return winrt::Windows::UI::Xaml::Controls::SettingsFlyout::TitleProperty(); } },
          { L"headerbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::SettingsFlyout::HeaderBackgroundProperty(); } },
          { L"headerforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::SettingsFlyout::HeaderForegroundProperty(); } },
          { L"iconsource"s, []() { return winrt::Windows::UI::Xaml::Controls::SettingsFlyout::IconSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Slider::IntermediateValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Slider>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"intermediatevalue"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::IntermediateValueProperty(); } },
          { L"stepfrequency"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::StepFrequencyProperty(); } },
          { L"snapsto"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::SnapsToProperty(); } },
          { L"tickfrequency"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::TickFrequencyProperty(); } },
          { L"tickplacement"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::TickPlacementProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::OrientationProperty(); } },
          { L"isdirectionreversed"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::IsDirectionReversedProperty(); } },
          { L"isthumbtooltipenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::IsThumbToolTipEnabledProperty(); } },
          { L"thumbtooltipvalueconverter"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::ThumbToolTipValueConverterProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::Slider::HeaderTemplateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SplitButton::FlyoutProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SplitButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"flyout"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitButton::FlyoutProperty(); } },
          { L"command"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitButton::CommandProperty(); } },
          { L"commandparameter"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitButton::CommandParameterProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SplitView::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SplitView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::ContentProperty(); } },
          { L"pane"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::PaneProperty(); } },
          { L"ispaneopen"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::IsPaneOpenProperty(); } },
          { L"openpanelength"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::OpenPaneLengthProperty(); } },
          { L"compactpanelength"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::CompactPaneLengthProperty(); } },
          { L"paneplacement"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::PanePlacementProperty(); } },
          { L"displaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::DisplayModeProperty(); } },
          { L"templatesettings"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::TemplateSettingsProperty(); } },
          { L"panebackground"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::PaneBackgroundProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::SplitView::LightDismissOverlayModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::StackPanel::AreScrollSnapPointsRegularProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::StackPanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"arescrollsnappointsregular"s, []() { return winrt::Windows::UI::Xaml::Controls::StackPanel::AreScrollSnapPointsRegularProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::StackPanel::OrientationProperty(); } },
          { L"borderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::StackPanel::BorderBrushProperty(); } },
          { L"borderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::StackPanel::BorderThicknessProperty(); } },
          { L"cornerradius"s, []() { return winrt::Windows::UI::Xaml::Controls::StackPanel::CornerRadiusProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::StackPanel::PaddingProperty(); } },
          { L"spacing"s, []() { return winrt::Windows::UI::Xaml::Controls::StackPanel::SpacingProperty(); } },
          { L"backgroundsizing"s, []() { return winrt::Windows::UI::Xaml::Controls::StackPanel::BackgroundSizingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SwapChainPanel::CompositionScaleXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SwapChainPanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"compositionscalex"s, []() { return winrt::Windows::UI::Xaml::Controls::SwapChainPanel::CompositionScaleXProperty(); } },
          { L"compositionscaley"s, []() { return winrt::Windows::UI::Xaml::Controls::SwapChainPanel::CompositionScaleYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SwipeControl::LeftItemsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SwipeControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"leftitems"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeControl::LeftItemsProperty(); } },
          { L"rightitems"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeControl::RightItemsProperty(); } },
          { L"topitems"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeControl::TopItemsProperty(); } },
          { L"bottomitems"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeControl::BottomItemsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SwipeItem::IconSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SwipeItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"iconsource"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeItem::IconSourceProperty(); } },
          { L"text"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeItem::TextProperty(); } },
          { L"background"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeItem::BackgroundProperty(); } },
          { L"foreground"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeItem::ForegroundProperty(); } },
          { L"command"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeItem::CommandProperty(); } },
          { L"commandparameter"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeItem::CommandParameterProperty(); } },
          { L"behavioroninvoked"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeItem::BehaviorOnInvokedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SwipeItems::ModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SwipeItems>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"mode"s, []() { return winrt::Windows::UI::Xaml::Controls::SwipeItems::ModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SymbolIcon::SymbolProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SymbolIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"symbol"s, []() { return winrt::Windows::UI::Xaml::Controls::SymbolIcon::SymbolProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::SymbolIconSource::SymbolProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::SymbolIconSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"symbol"s, []() { return winrt::Windows::UI::Xaml::Controls::SymbolIconSource::SymbolProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TextBlock::FontSizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TextBlock>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fontsize"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::FontStretchProperty(); } },
          { L"characterspacing"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::CharacterSpacingProperty(); } },
          { L"foreground"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::ForegroundProperty(); } },
          { L"textwrapping"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::TextWrappingProperty(); } },
          { L"texttrimming"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::TextTrimmingProperty(); } },
          { L"textalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::TextAlignmentProperty(); } },
          { L"text"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::TextProperty(); } },
          { L"padding"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::PaddingProperty(); } },
          { L"lineheight"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::LineHeightProperty(); } },
          { L"linestackingstrategy"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::LineStackingStrategyProperty(); } },
          { L"istextselectionenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::IsTextSelectionEnabledProperty(); } },
          { L"selectedtext"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::SelectedTextProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::SelectionHighlightColorProperty(); } },
          { L"maxlines"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::MaxLinesProperty(); } },
          { L"textlinebounds"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::TextLineBoundsProperty(); } },
          { L"opticalmarginalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::OpticalMarginAlignmentProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::IsColorFontEnabledProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::TextReadingOrderProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::IsTextScaleFactorEnabledProperty(); } },
          { L"textdecorations"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::TextDecorationsProperty(); } },
          { L"istexttrimmed"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::IsTextTrimmedProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::HorizontalTextAlignmentProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBlock::SelectionFlyoutProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TextBox::TextProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TextBox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"text"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::TextProperty(); } },
          { L"maxlength"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::MaxLengthProperty(); } },
          { L"isreadonly"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::IsReadOnlyProperty(); } },
          { L"acceptsreturn"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::AcceptsReturnProperty(); } },
          { L"textalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::TextAlignmentProperty(); } },
          { L"textwrapping"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::TextWrappingProperty(); } },
          { L"isspellcheckenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::IsSpellCheckEnabledProperty(); } },
          { L"istextpredictionenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::IsTextPredictionEnabledProperty(); } },
          { L"inputscope"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::InputScopeProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::HeaderTemplateProperty(); } },
          { L"placeholdertext"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::PlaceholderTextProperty(); } },
          { L"selectionhighlightcolor"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::SelectionHighlightColorProperty(); } },
          { L"preventkeyboarddisplayonprogrammaticfocus"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::PreventKeyboardDisplayOnProgrammaticFocusProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::IsColorFontEnabledProperty(); } },
          { L"desiredcandidatewindowalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::DesiredCandidateWindowAlignmentProperty(); } },
          { L"textreadingorder"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::TextReadingOrderProperty(); } },
          { L"selectionhighlightcolorwhennotfocused"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::SelectionHighlightColorWhenNotFocusedProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::HorizontalTextAlignmentProperty(); } },
          { L"charactercasing"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::CharacterCasingProperty(); } },
          { L"placeholderforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::PlaceholderForegroundProperty(); } },
          { L"handwritingview"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::HandwritingViewProperty(); } },
          { L"ishandwritingviewenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::IsHandwritingViewEnabledProperty(); } },
          { L"canpasteclipboardcontent"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::CanPasteClipboardContentProperty(); } },
          { L"canundo"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::CanUndoProperty(); } },
          { L"canredo"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::CanRedoProperty(); } },
          { L"selectionflyout"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::SelectionFlyoutProperty(); } },
          { L"proofingmenuflyout"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::ProofingMenuFlyoutProperty(); } },
          { L"description"s, []() { return winrt::Windows::UI::Xaml::Controls::TextBox::DescriptionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TimePicker::HeaderProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TimePicker>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePicker::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePicker::HeaderTemplateProperty(); } },
          { L"clockidentifier"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePicker::ClockIdentifierProperty(); } },
          { L"minuteincrement"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePicker::MinuteIncrementProperty(); } },
          { L"time"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePicker::TimeProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePicker::LightDismissOverlayModeProperty(); } },
          { L"selectedtime"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePicker::SelectedTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TimePickerFlyout::ClockIdentifierProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TimePickerFlyout>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"clockidentifier"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePickerFlyout::ClockIdentifierProperty(); } },
          { L"time"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePickerFlyout::TimeProperty(); } },
          { L"minuteincrement"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePickerFlyout::MinuteIncrementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TimePickerFlyoutPresenter::IsDefaultShadowEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TimePickerFlyoutPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdefaultshadowenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::TimePickerFlyoutPresenter::IsDefaultShadowEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem::IsCheckedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ischecked"s, []() { return winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem::IsCheckedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ToggleSwitch::IsOnProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ToggleSwitch>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ison"s, []() { return winrt::Windows::UI::Xaml::Controls::ToggleSwitch::IsOnProperty(); } },
          { L"header"s, []() { return winrt::Windows::UI::Xaml::Controls::ToggleSwitch::HeaderProperty(); } },
          { L"headertemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ToggleSwitch::HeaderTemplateProperty(); } },
          { L"oncontent"s, []() { return winrt::Windows::UI::Xaml::Controls::ToggleSwitch::OnContentProperty(); } },
          { L"oncontenttemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ToggleSwitch::OnContentTemplateProperty(); } },
          { L"offcontent"s, []() { return winrt::Windows::UI::Xaml::Controls::ToggleSwitch::OffContentProperty(); } },
          { L"offcontenttemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::ToggleSwitch::OffContentTemplateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ToolTip::HorizontalOffsetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ToolTip>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"horizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Controls::ToolTip::HorizontalOffsetProperty(); } },
          { L"isopen"s, []() { return winrt::Windows::UI::Xaml::Controls::ToolTip::IsOpenProperty(); } },
          { L"placement"s, []() { return winrt::Windows::UI::Xaml::Controls::ToolTip::PlacementProperty(); } },
          { L"placementtarget"s, []() { return winrt::Windows::UI::Xaml::Controls::ToolTip::PlacementTargetProperty(); } },
          { L"verticaloffset"s, []() { return winrt::Windows::UI::Xaml::Controls::ToolTip::VerticalOffsetProperty(); } },
          { L"placementrect"s, []() { return winrt::Windows::UI::Xaml::Controls::ToolTip::PlacementRectProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::ToolTipService::PlacementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::ToolTipService>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"placement"s, []() { return winrt::Windows::UI::Xaml::Controls::ToolTipService::PlacementProperty(); } },
          { L"placementtarget"s, []() { return winrt::Windows::UI::Xaml::Controls::ToolTipService::PlacementTargetProperty(); } },
          { L"tooltip"s, []() { return winrt::Windows::UI::Xaml::Controls::ToolTipService::ToolTipProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TreeView::SelectionModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TreeView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectionmode"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeView::SelectionModeProperty(); } },
          { L"candragitems"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeView::CanDragItemsProperty(); } },
          { L"canreorderitems"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeView::CanReorderItemsProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeView::ItemTemplateProperty(); } },
          { L"itemtemplateselector"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeView::ItemTemplateSelectorProperty(); } },
          { L"itemcontainerstyle"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeView::ItemContainerStyleProperty(); } },
          { L"itemcontainerstyleselector"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeView::ItemContainerStyleSelectorProperty(); } },
          { L"itemcontainertransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeView::ItemContainerTransitionsProperty(); } },
          { L"itemssource"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeView::ItemsSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TreeViewItem::GlyphOpacityProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TreeViewItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"glyphopacity"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItem::GlyphOpacityProperty(); } },
          { L"glyphbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItem::GlyphBrushProperty(); } },
          { L"expandedglyph"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItem::ExpandedGlyphProperty(); } },
          { L"collapsedglyph"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItem::CollapsedGlyphProperty(); } },
          { L"glyphsize"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItem::GlyphSizeProperty(); } },
          { L"isexpanded"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItem::IsExpandedProperty(); } },
          { L"treeviewitemtemplatesettings"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItem::TreeViewItemTemplateSettingsProperty(); } },
          { L"hasunrealizedchildren"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItem::HasUnrealizedChildrenProperty(); } },
          { L"itemssource"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItem::ItemsSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TreeViewItemTemplateSettings::ExpandedGlyphVisibilityProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TreeViewItemTemplateSettings>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"expandedglyphvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItemTemplateSettings::ExpandedGlyphVisibilityProperty(); } },
          { L"collapsedglyphvisibility"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItemTemplateSettings::CollapsedGlyphVisibilityProperty(); } },
          { L"indentation"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItemTemplateSettings::IndentationProperty(); } },
          { L"dragitemscount"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewItemTemplateSettings::DragItemsCountProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TreeViewNode::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TreeViewNode>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewNode::ContentProperty(); } },
          { L"depth"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewNode::DepthProperty(); } },
          { L"isexpanded"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewNode::IsExpandedProperty(); } },
          { L"haschildren"s, []() { return winrt::Windows::UI::Xaml::Controls::TreeViewNode::HasChildrenProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::TwoPaneView::Pane1Property())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::TwoPaneView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"pane1"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::Pane1Property(); } },
          { L"pane2"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::Pane2Property(); } },
          { L"pane1length"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::Pane1LengthProperty(); } },
          { L"pane2length"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::Pane2LengthProperty(); } },
          { L"panepriority"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::PanePriorityProperty(); } },
          { L"mode"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::ModeProperty(); } },
          { L"widemodeconfiguration"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::WideModeConfigurationProperty(); } },
          { L"tallmodeconfiguration"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::TallModeConfigurationProperty(); } },
          { L"minwidemodewidth"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::MinWideModeWidthProperty(); } },
          { L"mintallmodeheight"s, []() { return winrt::Windows::UI::Xaml::Controls::TwoPaneView::MinTallModeHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::UserControl::ContentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::UserControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"content"s, []() { return winrt::Windows::UI::Xaml::Controls::UserControl::ContentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid::ItemHeightProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemheight"s, []() { return winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid::ItemHeightProperty(); } },
          { L"itemwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid::ItemWidthProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid::OrientationProperty(); } },
          { L"horizontalchildrenalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid::HorizontalChildrenAlignmentProperty(); } },
          { L"verticalchildrenalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid::VerticalChildrenAlignmentProperty(); } },
          { L"maximumrowsorcolumns"s, []() { return winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid::MaximumRowsOrColumnsProperty(); } },
          { L"rowspan"s, []() { return winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid::RowSpanProperty(); } },
          { L"columnspan"s, []() { return winrt::Windows::UI::Xaml::Controls::VariableSizedWrapGrid::ColumnSpanProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Viewbox::StretchProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Viewbox>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"stretch"s, []() { return winrt::Windows::UI::Xaml::Controls::Viewbox::StretchProperty(); } },
          { L"stretchdirection"s, []() { return winrt::Windows::UI::Xaml::Controls::Viewbox::StretchDirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::VirtualizingStackPanel::AreScrollSnapPointsRegularProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::VirtualizingStackPanel>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"arescrollsnappointsregular"s, []() { return winrt::Windows::UI::Xaml::Controls::VirtualizingStackPanel::AreScrollSnapPointsRegularProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::VirtualizingStackPanel::OrientationProperty(); } },
          { L"virtualizationmode"s, []() { return winrt::Windows::UI::Xaml::Controls::VirtualizingStackPanel::VirtualizationModeProperty(); } },
          { L"isvirtualizing"s, []() { return winrt::Windows::UI::Xaml::Controls::VirtualizingStackPanel::IsVirtualizingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::WebView::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::WebView>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::SourceProperty(); } },
          { L"allowedscriptnotifyuris"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::AllowedScriptNotifyUrisProperty(); } },
          { L"datatransferpackage"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::DataTransferPackageProperty(); } },
          { L"cangoback"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::CanGoBackProperty(); } },
          { L"cangoforward"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::CanGoForwardProperty(); } },
          { L"documenttitle"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::DocumentTitleProperty(); } },
          { L"defaultbackgroundcolor"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::DefaultBackgroundColorProperty(); } },
          { L"containsfullscreenelement"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::ContainsFullScreenElementProperty(); } },
          { L"xyfocusleft"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::XYFocusLeftProperty(); } },
          { L"xyfocusright"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::XYFocusRightProperty(); } },
          { L"xyfocusup"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::XYFocusUpProperty(); } },
          { L"xyfocusdown"s, []() { return winrt::Windows::UI::Xaml::Controls::WebView::XYFocusDownProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::WebViewBrush::SourceNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::WebViewBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"sourcename"s, []() { return winrt::Windows::UI::Xaml::Controls::WebViewBrush::SourceNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::WrapGrid::ItemWidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::WrapGrid>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::WrapGrid::ItemWidthProperty(); } },
          { L"itemheight"s, []() { return winrt::Windows::UI::Xaml::Controls::WrapGrid::ItemHeightProperty(); } },
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::WrapGrid::OrientationProperty(); } },
          { L"horizontalchildrenalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::WrapGrid::HorizontalChildrenAlignmentProperty(); } },
          { L"verticalchildrenalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::WrapGrid::VerticalChildrenAlignmentProperty(); } },
          { L"maximumrowsorcolumns"s, []() { return winrt::Windows::UI::Xaml::Controls::WrapGrid::MaximumRowsOrColumnsProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Controls.Maps.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapBillboard::LocationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapBillboard>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"location"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapBillboard::LocationProperty(); } },
          { L"normalizedanchorpoint"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapBillboard::NormalizedAnchorPointProperty(); } },
          { L"collisionbehaviordesired"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapBillboard::CollisionBehaviorDesiredProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapControl::CenterProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"center"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::CenterProperty(); } },
          { L"children"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::ChildrenProperty(); } },
          { L"colorscheme"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::ColorSchemeProperty(); } },
          { L"desiredpitch"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::DesiredPitchProperty(); } },
          { L"heading"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::HeadingProperty(); } },
          { L"landmarksvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::LandmarksVisibleProperty(); } },
          { L"loadingstatus"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::LoadingStatusProperty(); } },
          { L"mapservicetoken"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::MapServiceTokenProperty(); } },
          { L"pedestrianfeaturesvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::PedestrianFeaturesVisibleProperty(); } },
          { L"pitch"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::PitchProperty(); } },
          { L"style"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::StyleProperty(); } },
          { L"trafficflowvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::TrafficFlowVisibleProperty(); } },
          { L"transformorigin"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::TransformOriginProperty(); } },
          { L"watermarkmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::WatermarkModeProperty(); } },
          { L"zoomlevel"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::ZoomLevelProperty(); } },
          { L"mapelements"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::MapElementsProperty(); } },
          { L"routes"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::RoutesProperty(); } },
          { L"tilesources"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::TileSourcesProperty(); } },
          { L"location"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::LocationProperty(); } },
          { L"normalizedanchorpoint"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::NormalizedAnchorPointProperty(); } },
          { L"businesslandmarksvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::BusinessLandmarksVisibleProperty(); } },
          { L"transitfeaturesvisible"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::TransitFeaturesVisibleProperty(); } },
          { L"paninteractionmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::PanInteractionModeProperty(); } },
          { L"rotateinteractionmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::RotateInteractionModeProperty(); } },
          { L"tiltinteractionmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::TiltInteractionModeProperty(); } },
          { L"zoominteractionmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::ZoomInteractionModeProperty(); } },
          { L"is3dsupported"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::Is3DSupportedProperty(); } },
          { L"isstreetsidesupported"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::IsStreetsideSupportedProperty(); } },
          { L"scene"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::SceneProperty(); } },
          { L"businesslandmarksenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::BusinessLandmarksEnabledProperty(); } },
          { L"transitfeaturesenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::TransitFeaturesEnabledProperty(); } },
          { L"mapprojection"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::MapProjectionProperty(); } },
          { L"stylesheet"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::StyleSheetProperty(); } },
          { L"viewpadding"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::ViewPaddingProperty(); } },
          { L"layers"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::LayersProperty(); } },
          { L"region"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::RegionProperty(); } },
          { L"cantiltdown"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::CanTiltDownProperty(); } },
          { L"cantiltup"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::CanTiltUpProperty(); } },
          { L"canzoomin"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::CanZoomInProperty(); } },
          { L"canzoomout"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapControl::CanZoomOutProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapElement::ZIndexProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"zindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement::ZIndexProperty(); } },
          { L"visible"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement::VisibleProperty(); } },
          { L"maptabindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement::MapTabIndexProperty(); } },
          { L"mapstylesheetentry"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement::MapStyleSheetEntryProperty(); } },
          { L"mapstylesheetentrystate"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement::MapStyleSheetEntryStateProperty(); } },
          { L"tag"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement::TagProperty(); } },
          { L"isenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement::IsEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapElement3D::LocationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapElement3D>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"location"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement3D::LocationProperty(); } },
          { L"heading"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement3D::HeadingProperty(); } },
          { L"pitch"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement3D::PitchProperty(); } },
          { L"roll"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement3D::RollProperty(); } },
          { L"scale"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElement3D::ScaleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapElementsLayer::MapElementsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapElementsLayer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"mapelements"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapElementsLayer::MapElementsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapIcon::LocationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapIcon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"location"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapIcon::LocationProperty(); } },
          { L"title"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapIcon::TitleProperty(); } },
          { L"normalizedanchorpoint"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapIcon::NormalizedAnchorPointProperty(); } },
          { L"collisionbehaviordesired"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapIcon::CollisionBehaviorDesiredProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapItemsControl::ItemsSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapItemsControl>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"itemssource"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapItemsControl::ItemsSourceProperty(); } },
          { L"items"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapItemsControl::ItemsProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapItemsControl::ItemTemplateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapLayer::MapTabIndexProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapLayer>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"maptabindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapLayer::MapTabIndexProperty(); } },
          { L"visible"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapLayer::VisibleProperty(); } },
          { L"zindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapLayer::ZIndexProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapPolygon::PathProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapPolygon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"path"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapPolygon::PathProperty(); } },
          { L"strokethickness"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapPolygon::StrokeThicknessProperty(); } },
          { L"strokedashed"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapPolygon::StrokeDashedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapPolyline::PathProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapPolyline>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"path"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapPolyline::PathProperty(); } },
          { L"strokedashed"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapPolyline::StrokeDashedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::DataSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"datasource"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::DataSourceProperty(); } },
          { L"layer"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::LayerProperty(); } },
          { L"zoomlevelrange"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::ZoomLevelRangeProperty(); } },
          { L"bounds"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::BoundsProperty(); } },
          { L"allowoverstretch"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::AllowOverstretchProperty(); } },
          { L"isfadingenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::IsFadingEnabledProperty(); } },
          { L"istransparencyenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::IsTransparencyEnabledProperty(); } },
          { L"isretryenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::IsRetryEnabledProperty(); } },
          { L"zindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::ZIndexProperty(); } },
          { L"tilepixelsize"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::TilePixelSizeProperty(); } },
          { L"visible"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::VisibleProperty(); } },
          { L"animationstate"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::AnimationStateProperty(); } },
          { L"autoplay"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::AutoPlayProperty(); } },
          { L"framecount"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::FrameCountProperty(); } },
          { L"frameduration"s, []() { return winrt::Windows::UI::Xaml::Controls::Maps::MapTileSource::FrameDurationProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Controls.Primitives.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::ButtonBase::ClickModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::ButtonBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"clickmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ButtonBase::ClickModeProperty(); } },
          { L"ispointerover"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ButtonBase::IsPointerOverProperty(); } },
          { L"ispressed"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ButtonBase::IsPressedProperty(); } },
          { L"command"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ButtonBase::CommandProperty(); } },
          { L"commandparameter"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ButtonBase::CommandParameterProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::ColorPickerSlider::ColorChannelProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::ColorPickerSlider>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"colorchannel"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorPickerSlider::ColorChannelProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::ColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"color"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::ColorProperty(); } },
          { L"hsvcolor"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::HsvColorProperty(); } },
          { L"minhue"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::MinHueProperty(); } },
          { L"maxhue"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::MaxHueProperty(); } },
          { L"minsaturation"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::MinSaturationProperty(); } },
          { L"maxsaturation"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::MaxSaturationProperty(); } },
          { L"minvalue"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::MinValueProperty(); } },
          { L"maxvalue"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::MaxValueProperty(); } },
          { L"shape"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::ShapeProperty(); } },
          { L"components"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ColorSpectrum::ComponentsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::PlacementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"placement"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::PlacementProperty(); } },
          { L"attachedflyout"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::AttachedFlyoutProperty(); } },
          { L"allowfocusoninteraction"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::AllowFocusOnInteractionProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::LightDismissOverlayModeProperty(); } },
          { L"allowfocuswhendisabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::AllowFocusWhenDisabledProperty(); } },
          { L"elementsoundmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::ElementSoundModeProperty(); } },
          { L"overlayinputpassthroughelement"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::OverlayInputPassThroughElementProperty(); } },
          { L"target"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::TargetProperty(); } },
          { L"showmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::ShowModeProperty(); } },
          { L"inputdeviceprefersprimarycommands"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::InputDevicePrefersPrimaryCommandsProperty(); } },
          { L"areopencloseanimationsenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::AreOpenCloseAnimationsEnabledProperty(); } },
          { L"isopen"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::IsOpenProperty(); } },
          { L"shouldconstraintorootbounds"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase::ShouldConstrainToRootBoundsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectionCheckMarkVisualEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectioncheckmarkvisualenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectionCheckMarkVisualEnabledProperty(); } },
          { L"checkhintbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::CheckHintBrushProperty(); } },
          { L"checkselectingbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::CheckSelectingBrushProperty(); } },
          { L"checkbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::CheckBrushProperty(); } },
          { L"dragbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::DragBackgroundProperty(); } },
          { L"dragforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::DragForegroundProperty(); } },
          { L"focusborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::FocusBorderBrushProperty(); } },
          { L"placeholderbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::PlaceholderBackgroundProperty(); } },
          { L"pointeroverbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::PointerOverBackgroundProperty(); } },
          { L"selectedbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedBackgroundProperty(); } },
          { L"selectedforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedForegroundProperty(); } },
          { L"selectedpointeroverbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedPointerOverBackgroundProperty(); } },
          { L"selectedpointeroverborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedPointerOverBorderBrushProperty(); } },
          { L"selectedborderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::SelectedBorderThicknessProperty(); } },
          { L"disabledopacity"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::DisabledOpacityProperty(); } },
          { L"dragopacity"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::DragOpacityProperty(); } },
          { L"reorderhintoffset"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::ReorderHintOffsetProperty(); } },
          { L"gridviewitempresenterhorizontalcontentalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::GridViewItemPresenterHorizontalContentAlignmentProperty(); } },
          { L"gridviewitempresenterverticalcontentalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::GridViewItemPresenterVerticalContentAlignmentProperty(); } },
          { L"gridviewitempresenterpadding"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::GridViewItemPresenterPaddingProperty(); } },
          { L"pointeroverbackgroundmargin"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::PointerOverBackgroundMarginProperty(); } },
          { L"contentmargin"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::GridViewItemPresenter::ContentMarginProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::JumpListItemBackgroundConverter::EnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::JumpListItemBackgroundConverter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::JumpListItemBackgroundConverter::EnabledProperty(); } },
          { L"disabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::JumpListItemBackgroundConverter::DisabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::JumpListItemForegroundConverter::EnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::JumpListItemForegroundConverter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::JumpListItemForegroundConverter::EnabledProperty(); } },
          { L"disabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::JumpListItemForegroundConverter::DisabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionCheckMarkVisualEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectioncheckmarkvisualenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionCheckMarkVisualEnabledProperty(); } },
          { L"checkhintbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckHintBrushProperty(); } },
          { L"checkselectingbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckSelectingBrushProperty(); } },
          { L"checkbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBrushProperty(); } },
          { L"dragbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::DragBackgroundProperty(); } },
          { L"dragforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::DragForegroundProperty(); } },
          { L"focusborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::FocusBorderBrushProperty(); } },
          { L"placeholderbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PlaceholderBackgroundProperty(); } },
          { L"pointeroverbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PointerOverBackgroundProperty(); } },
          { L"selectedbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedBackgroundProperty(); } },
          { L"selectedforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedForegroundProperty(); } },
          { L"selectedpointeroverbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedPointerOverBackgroundProperty(); } },
          { L"selectedpointeroverborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedPointerOverBorderBrushProperty(); } },
          { L"selectedborderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedBorderThicknessProperty(); } },
          { L"disabledopacity"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::DisabledOpacityProperty(); } },
          { L"dragopacity"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::DragOpacityProperty(); } },
          { L"reorderhintoffset"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ReorderHintOffsetProperty(); } },
          { L"listviewitempresenterhorizontalcontentalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ListViewItemPresenterHorizontalContentAlignmentProperty(); } },
          { L"listviewitempresenterverticalcontentalignment"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ListViewItemPresenterVerticalContentAlignmentProperty(); } },
          { L"listviewitempresenterpadding"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ListViewItemPresenterPaddingProperty(); } },
          { L"pointeroverbackgroundmargin"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PointerOverBackgroundMarginProperty(); } },
          { L"contentmargin"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::ContentMarginProperty(); } },
          { L"selectedpressedbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedPressedBackgroundProperty(); } },
          { L"pressedbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PressedBackgroundProperty(); } },
          { L"checkboxbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxBrushProperty(); } },
          { L"focussecondaryborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::FocusSecondaryBorderBrushProperty(); } },
          { L"checkmode"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckModeProperty(); } },
          { L"pointeroverforeground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PointerOverForegroundProperty(); } },
          { L"revealbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::RevealBackgroundProperty(); } },
          { L"revealborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::RevealBorderBrushProperty(); } },
          { L"revealborderthickness"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::RevealBorderThicknessProperty(); } },
          { L"revealbackgroundshowsabovecontent"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::RevealBackgroundShowsAboveContentProperty(); } },
          { L"selecteddisabledbackground"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedDisabledBackgroundProperty(); } },
          { L"checkpressedbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckPressedBrushProperty(); } },
          { L"checkdisabledbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckDisabledBrushProperty(); } },
          { L"checkboxpointeroverbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxPointerOverBrushProperty(); } },
          { L"checkboxpressedbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxPressedBrushProperty(); } },
          { L"checkboxdisabledbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxDisabledBrushProperty(); } },
          { L"checkboxselectedbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxSelectedBrushProperty(); } },
          { L"checkboxselectedpointeroverbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxSelectedPointerOverBrushProperty(); } },
          { L"checkboxselectedpressedbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxSelectedPressedBrushProperty(); } },
          { L"checkboxselecteddisabledbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxSelectedDisabledBrushProperty(); } },
          { L"checkboxborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxBorderBrushProperty(); } },
          { L"checkboxpointeroverborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxPointerOverBorderBrushProperty(); } },
          { L"checkboxpressedborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxPressedBorderBrushProperty(); } },
          { L"checkboxdisabledborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxDisabledBorderBrushProperty(); } },
          { L"checkboxcornerradius"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::CheckBoxCornerRadiusProperty(); } },
          { L"selectionindicatorcornerradius"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorCornerRadiusProperty(); } },
          { L"selectionindicatorvisualenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorVisualEnabledProperty(); } },
          { L"selectionindicatormode"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorModeProperty(); } },
          { L"selectionindicatorbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorBrushProperty(); } },
          { L"selectionindicatorpointeroverbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorPointerOverBrushProperty(); } },
          { L"selectionindicatorpressedbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorPressedBrushProperty(); } },
          { L"selectionindicatordisabledbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectionIndicatorDisabledBrushProperty(); } },
          { L"selectedborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedBorderBrushProperty(); } },
          { L"selectedpressedborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedPressedBorderBrushProperty(); } },
          { L"selecteddisabledborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedDisabledBorderBrushProperty(); } },
          { L"selectedinnerborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::SelectedInnerBorderBrushProperty(); } },
          { L"pointeroverborderbrush"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter::PointerOverBorderBrushProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::LoopingSelector::ShouldLoopProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::LoopingSelector>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"shouldloop"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::LoopingSelector::ShouldLoopProperty(); } },
          { L"items"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::LoopingSelector::ItemsProperty(); } },
          { L"selectedindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::LoopingSelector::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::LoopingSelector::SelectedItemProperty(); } },
          { L"itemwidth"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::LoopingSelector::ItemWidthProperty(); } },
          { L"itemheight"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::LoopingSelector::ItemHeightProperty(); } },
          { L"itemtemplate"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::LoopingSelector::ItemTemplateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::NavigationViewItemPresenter::IconProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::NavigationViewItemPresenter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"icon"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::NavigationViewItemPresenter::IconProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::PickerFlyoutBase::TitleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::PickerFlyoutBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"title"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::PickerFlyoutBase::TitleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::Popup::ChildProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::Popup>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"child"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::ChildProperty(); } },
          { L"isopen"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::IsOpenProperty(); } },
          { L"horizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::HorizontalOffsetProperty(); } },
          { L"verticaloffset"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::VerticalOffsetProperty(); } },
          { L"childtransitions"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::ChildTransitionsProperty(); } },
          { L"islightdismissenabled"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::IsLightDismissEnabledProperty(); } },
          { L"lightdismissoverlaymode"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::LightDismissOverlayModeProperty(); } },
          { L"shouldconstraintorootbounds"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::ShouldConstrainToRootBoundsProperty(); } },
          { L"placementtarget"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::PlacementTargetProperty(); } },
          { L"desiredplacement"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Popup::DesiredPlacementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase::MinimumProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"minimum"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase::MinimumProperty(); } },
          { L"maximum"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase::MaximumProperty(); } },
          { L"smallchange"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase::SmallChangeProperty(); } },
          { L"largechange"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase::LargeChangeProperty(); } },
          { L"value"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase::ValueProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton::DelayProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"delay"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton::DelayProperty(); } },
          { L"interval"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton::IntervalProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::ScrollBar::OrientationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::ScrollBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"orientation"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ScrollBar::OrientationProperty(); } },
          { L"viewportsize"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ScrollBar::ViewportSizeProperty(); } },
          { L"indicatormode"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ScrollBar::IndicatorModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::Selector::SelectedIndexProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::Selector>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"selectedindex"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Selector::SelectedIndexProperty(); } },
          { L"selecteditem"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Selector::SelectedItemProperty(); } },
          { L"selectedvalue"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Selector::SelectedValueProperty(); } },
          { L"selectedvaluepath"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Selector::SelectedValuePathProperty(); } },
          { L"issynchronizedwithcurrentitem"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Selector::IsSynchronizedWithCurrentItemProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::SelectorItem::IsSelectedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::SelectorItem>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isselected"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::SelectorItem::IsSelectedProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::Thumb::IsDraggingProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::Thumb>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isdragging"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::Thumb::IsDraggingProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::TickBar::FillProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::TickBar>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fill"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::TickBar::FillProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton::IsCheckedProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"ischecked"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton::IsCheckedProperty(); } },
          { L"isthreestate"s, []() { return winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton::IsThreeStateProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Data.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Data::CollectionViewSource::SourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Data::CollectionViewSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"source"s, []() { return winrt::Windows::UI::Xaml::Data::CollectionViewSource::SourceProperty(); } },
          { L"view"s, []() { return winrt::Windows::UI::Xaml::Data::CollectionViewSource::ViewProperty(); } },
          { L"issourcegrouped"s, []() { return winrt::Windows::UI::Xaml::Data::CollectionViewSource::IsSourceGroupedProperty(); } },
          { L"itemspath"s, []() { return winrt::Windows::UI::Xaml::Data::CollectionViewSource::ItemsPathProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Documents.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Documents::Block::TextAlignmentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Documents::Block>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"textalignment"s, []() { return winrt::Windows::UI::Xaml::Documents::Block::TextAlignmentProperty(); } },
          { L"lineheight"s, []() { return winrt::Windows::UI::Xaml::Documents::Block::LineHeightProperty(); } },
          { L"linestackingstrategy"s, []() { return winrt::Windows::UI::Xaml::Documents::Block::LineStackingStrategyProperty(); } },
          { L"margin"s, []() { return winrt::Windows::UI::Xaml::Documents::Block::MarginProperty(); } },
          { L"horizontaltextalignment"s, []() { return winrt::Windows::UI::Xaml::Documents::Block::HorizontalTextAlignmentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Documents::ContentLink::BackgroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Documents::ContentLink>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"background"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::BackgroundProperty(); } },
          { L"cursor"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::CursorProperty(); } },
          { L"xyfocusleft"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::XYFocusLeftProperty(); } },
          { L"xyfocusright"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::XYFocusRightProperty(); } },
          { L"xyfocusup"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::XYFocusUpProperty(); } },
          { L"xyfocusdown"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::XYFocusDownProperty(); } },
          { L"elementsoundmode"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::ElementSoundModeProperty(); } },
          { L"focusstate"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::FocusStateProperty(); } },
          { L"xyfocusupnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::XYFocusUpNavigationStrategyProperty(); } },
          { L"xyfocusdownnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::XYFocusDownNavigationStrategyProperty(); } },
          { L"xyfocusleftnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::XYFocusLeftNavigationStrategyProperty(); } },
          { L"xyfocusrightnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::XYFocusRightNavigationStrategyProperty(); } },
          { L"istabstop"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::IsTabStopProperty(); } },
          { L"tabindex"s, []() { return winrt::Windows::UI::Xaml::Documents::ContentLink::TabIndexProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Documents::Glyphs::UnicodeStringProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Documents::Glyphs>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"unicodestring"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::UnicodeStringProperty(); } },
          { L"indices"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::IndicesProperty(); } },
          { L"fonturi"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::FontUriProperty(); } },
          { L"stylesimulations"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::StyleSimulationsProperty(); } },
          { L"fontrenderingemsize"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::FontRenderingEmSizeProperty(); } },
          { L"originx"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::OriginXProperty(); } },
          { L"originy"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::OriginYProperty(); } },
          { L"fill"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::FillProperty(); } },
          { L"iscolorfontenabled"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::IsColorFontEnabledProperty(); } },
          { L"colorfontpaletteindex"s, []() { return winrt::Windows::UI::Xaml::Documents::Glyphs::ColorFontPaletteIndexProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Documents::Hyperlink::NavigateUriProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Documents::Hyperlink>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"navigateuri"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::NavigateUriProperty(); } },
          { L"underlinestyle"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::UnderlineStyleProperty(); } },
          { L"xyfocusleft"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::XYFocusLeftProperty(); } },
          { L"xyfocusright"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::XYFocusRightProperty(); } },
          { L"xyfocusup"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::XYFocusUpProperty(); } },
          { L"xyfocusdown"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::XYFocusDownProperty(); } },
          { L"elementsoundmode"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::ElementSoundModeProperty(); } },
          { L"focusstate"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::FocusStateProperty(); } },
          { L"xyfocusupnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::XYFocusUpNavigationStrategyProperty(); } },
          { L"xyfocusdownnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::XYFocusDownNavigationStrategyProperty(); } },
          { L"xyfocusleftnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::XYFocusLeftNavigationStrategyProperty(); } },
          { L"xyfocusrightnavigationstrategy"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::XYFocusRightNavigationStrategyProperty(); } },
          { L"istabstop"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::IsTabStopProperty(); } },
          { L"tabindex"s, []() { return winrt::Windows::UI::Xaml::Documents::Hyperlink::TabIndexProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Documents::Paragraph::TextIndentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Documents::Paragraph>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"textindent"s, []() { return winrt::Windows::UI::Xaml::Documents::Paragraph::TextIndentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Documents::Run::FlowDirectionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Documents::Run>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"flowdirection"s, []() { return winrt::Windows::UI::Xaml::Documents::Run::FlowDirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Documents::TextElement::FontSizeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Documents::TextElement>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fontsize"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::FontSizeProperty(); } },
          { L"fontfamily"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::FontFamilyProperty(); } },
          { L"fontweight"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::FontWeightProperty(); } },
          { L"fontstyle"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::FontStyleProperty(); } },
          { L"fontstretch"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::FontStretchProperty(); } },
          { L"characterspacing"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::CharacterSpacingProperty(); } },
          { L"foreground"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::ForegroundProperty(); } },
          { L"language"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::LanguageProperty(); } },
          { L"istextscalefactorenabled"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::IsTextScaleFactorEnabledProperty(); } },
          { L"allowfocusoninteraction"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::AllowFocusOnInteractionProperty(); } },
          { L"accesskey"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::AccessKeyProperty(); } },
          { L"exitdisplaymodeonaccesskeyinvoked"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::ExitDisplayModeOnAccessKeyInvokedProperty(); } },
          { L"textdecorations"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::TextDecorationsProperty(); } },
          { L"isaccesskeyscope"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::IsAccessKeyScopeProperty(); } },
          { L"accesskeyscopeowner"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::AccessKeyScopeOwnerProperty(); } },
          { L"keytipplacementmode"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::KeyTipPlacementModeProperty(); } },
          { L"keytiphorizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::KeyTipHorizontalOffsetProperty(); } },
          { L"keytipverticaloffset"s, []() { return winrt::Windows::UI::Xaml::Documents::TextElement::KeyTipVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Documents::TextHighlighter::ForegroundProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Documents::TextHighlighter>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"foreground"s, []() { return winrt::Windows::UI::Xaml::Documents::TextHighlighter::ForegroundProperty(); } },
          { L"background"s, []() { return winrt::Windows::UI::Xaml::Documents::TextHighlighter::BackgroundProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Documents::Typography::AnnotationAlternatesProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Documents::Typography>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"annotationalternates"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::AnnotationAlternatesProperty(); } },
          { L"eastasianexpertforms"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::EastAsianExpertFormsProperty(); } },
          { L"eastasianlanguage"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::EastAsianLanguageProperty(); } },
          { L"eastasianwidths"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::EastAsianWidthsProperty(); } },
          { L"standardligatures"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StandardLigaturesProperty(); } },
          { L"contextualligatures"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::ContextualLigaturesProperty(); } },
          { L"discretionaryligatures"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::DiscretionaryLigaturesProperty(); } },
          { L"historicalligatures"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::HistoricalLigaturesProperty(); } },
          { L"standardswashes"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StandardSwashesProperty(); } },
          { L"contextualswashes"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::ContextualSwashesProperty(); } },
          { L"contextualalternates"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::ContextualAlternatesProperty(); } },
          { L"stylisticalternates"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticAlternatesProperty(); } },
          { L"stylisticset1"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet1Property(); } },
          { L"stylisticset2"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet2Property(); } },
          { L"stylisticset3"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet3Property(); } },
          { L"stylisticset4"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet4Property(); } },
          { L"stylisticset5"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet5Property(); } },
          { L"stylisticset6"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet6Property(); } },
          { L"stylisticset7"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet7Property(); } },
          { L"stylisticset8"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet8Property(); } },
          { L"stylisticset9"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet9Property(); } },
          { L"stylisticset10"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet10Property(); } },
          { L"stylisticset11"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet11Property(); } },
          { L"stylisticset12"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet12Property(); } },
          { L"stylisticset13"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet13Property(); } },
          { L"stylisticset14"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet14Property(); } },
          { L"stylisticset15"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet15Property(); } },
          { L"stylisticset16"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet16Property(); } },
          { L"stylisticset17"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet17Property(); } },
          { L"stylisticset18"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet18Property(); } },
          { L"stylisticset19"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet19Property(); } },
          { L"stylisticset20"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::StylisticSet20Property(); } },
          { L"capitals"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::CapitalsProperty(); } },
          { L"capitalspacing"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::CapitalSpacingProperty(); } },
          { L"kerning"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::KerningProperty(); } },
          { L"casesensitiveforms"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::CaseSensitiveFormsProperty(); } },
          { L"historicalforms"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::HistoricalFormsProperty(); } },
          { L"fraction"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::FractionProperty(); } },
          { L"numeralstyle"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::NumeralStyleProperty(); } },
          { L"numeralalignment"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::NumeralAlignmentProperty(); } },
          { L"slashedzero"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::SlashedZeroProperty(); } },
          { L"mathematicalgreek"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::MathematicalGreekProperty(); } },
          { L"variants"s, []() { return winrt::Windows::UI::Xaml::Documents::Typography::VariantsProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Input.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Input::KeyboardAccelerator::KeyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Input::KeyboardAccelerator>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"key"s, []() { return winrt::Windows::UI::Xaml::Input::KeyboardAccelerator::KeyProperty(); } },
          { L"modifiers"s, []() { return winrt::Windows::UI::Xaml::Input::KeyboardAccelerator::ModifiersProperty(); } },
          { L"isenabled"s, []() { return winrt::Windows::UI::Xaml::Input::KeyboardAccelerator::IsEnabledProperty(); } },
          { L"scopeowner"s, []() { return winrt::Windows::UI::Xaml::Input::KeyboardAccelerator::ScopeOwnerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Input::StandardUICommand::KindProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Input::StandardUICommand>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"kind"s, []() { return winrt::Windows::UI::Xaml::Input::StandardUICommand::KindProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Input::XamlUICommand::LabelProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Input::XamlUICommand>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"label"s, []() { return winrt::Windows::UI::Xaml::Input::XamlUICommand::LabelProperty(); } },
          { L"iconsource"s, []() { return winrt::Windows::UI::Xaml::Input::XamlUICommand::IconSourceProperty(); } },
          { L"keyboardaccelerators"s, []() { return winrt::Windows::UI::Xaml::Input::XamlUICommand::KeyboardAcceleratorsProperty(); } },
          { L"accesskey"s, []() { return winrt::Windows::UI::Xaml::Input::XamlUICommand::AccessKeyProperty(); } },
          { L"description"s, []() { return winrt::Windows::UI::Xaml::Input::XamlUICommand::DescriptionProperty(); } },
          { L"command"s, []() { return winrt::Windows::UI::Xaml::Input::XamlUICommand::CommandProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Markup.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Markup::XamlBindingHelper::DataTemplateComponentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Markup::XamlBindingHelper>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"datatemplatecomponent"s, []() { return winrt::Windows::UI::Xaml::Markup::XamlBindingHelper::DataTemplateComponentProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Media.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::AcrylicBrush::BackgroundSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::AcrylicBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"backgroundsource"s, []() { return winrt::Windows::UI::Xaml::Media::AcrylicBrush::BackgroundSourceProperty(); } },
          { L"tintcolor"s, []() { return winrt::Windows::UI::Xaml::Media::AcrylicBrush::TintColorProperty(); } },
          { L"tintopacity"s, []() { return winrt::Windows::UI::Xaml::Media::AcrylicBrush::TintOpacityProperty(); } },
          { L"tinttransitionduration"s, []() { return winrt::Windows::UI::Xaml::Media::AcrylicBrush::TintTransitionDurationProperty(); } },
          { L"alwaysusefallback"s, []() { return winrt::Windows::UI::Xaml::Media::AcrylicBrush::AlwaysUseFallbackProperty(); } },
          { L"tintluminosityopacity"s, []() { return winrt::Windows::UI::Xaml::Media::AcrylicBrush::TintLuminosityOpacityProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::ArcSegment::PointProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::ArcSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"point"s, []() { return winrt::Windows::UI::Xaml::Media::ArcSegment::PointProperty(); } },
          { L"size"s, []() { return winrt::Windows::UI::Xaml::Media::ArcSegment::SizeProperty(); } },
          { L"rotationangle"s, []() { return winrt::Windows::UI::Xaml::Media::ArcSegment::RotationAngleProperty(); } },
          { L"islargearc"s, []() { return winrt::Windows::UI::Xaml::Media::ArcSegment::IsLargeArcProperty(); } },
          { L"sweepdirection"s, []() { return winrt::Windows::UI::Xaml::Media::ArcSegment::SweepDirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::BezierSegment::Point1Property())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::BezierSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"point1"s, []() { return winrt::Windows::UI::Xaml::Media::BezierSegment::Point1Property(); } },
          { L"point2"s, []() { return winrt::Windows::UI::Xaml::Media::BezierSegment::Point2Property(); } },
          { L"point3"s, []() { return winrt::Windows::UI::Xaml::Media::BezierSegment::Point3Property(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Brush::OpacityProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Brush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"opacity"s, []() { return winrt::Windows::UI::Xaml::Media::Brush::OpacityProperty(); } },
          { L"transform"s, []() { return winrt::Windows::UI::Xaml::Media::Brush::TransformProperty(); } },
          { L"relativetransform"s, []() { return winrt::Windows::UI::Xaml::Media::Brush::RelativeTransformProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::CompositeTransform::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::CompositeTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Windows::UI::Xaml::Media::CompositeTransform::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Windows::UI::Xaml::Media::CompositeTransform::CenterYProperty(); } },
          { L"scalex"s, []() { return winrt::Windows::UI::Xaml::Media::CompositeTransform::ScaleXProperty(); } },
          { L"scaley"s, []() { return winrt::Windows::UI::Xaml::Media::CompositeTransform::ScaleYProperty(); } },
          { L"skewx"s, []() { return winrt::Windows::UI::Xaml::Media::CompositeTransform::SkewXProperty(); } },
          { L"skewy"s, []() { return winrt::Windows::UI::Xaml::Media::CompositeTransform::SkewYProperty(); } },
          { L"rotation"s, []() { return winrt::Windows::UI::Xaml::Media::CompositeTransform::RotationProperty(); } },
          { L"translatex"s, []() { return winrt::Windows::UI::Xaml::Media::CompositeTransform::TranslateXProperty(); } },
          { L"translatey"s, []() { return winrt::Windows::UI::Xaml::Media::CompositeTransform::TranslateYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::EllipseGeometry::CenterProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::EllipseGeometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"center"s, []() { return winrt::Windows::UI::Xaml::Media::EllipseGeometry::CenterProperty(); } },
          { L"radiusx"s, []() { return winrt::Windows::UI::Xaml::Media::EllipseGeometry::RadiusXProperty(); } },
          { L"radiusy"s, []() { return winrt::Windows::UI::Xaml::Media::EllipseGeometry::RadiusYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Geometry::TransformProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Geometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"transform"s, []() { return winrt::Windows::UI::Xaml::Media::Geometry::TransformProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::GeometryGroup::FillRuleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::GeometryGroup>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fillrule"s, []() { return winrt::Windows::UI::Xaml::Media::GeometryGroup::FillRuleProperty(); } },
          { L"children"s, []() { return winrt::Windows::UI::Xaml::Media::GeometryGroup::ChildrenProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::GradientBrush::SpreadMethodProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::GradientBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"spreadmethod"s, []() { return winrt::Windows::UI::Xaml::Media::GradientBrush::SpreadMethodProperty(); } },
          { L"mappingmode"s, []() { return winrt::Windows::UI::Xaml::Media::GradientBrush::MappingModeProperty(); } },
          { L"colorinterpolationmode"s, []() { return winrt::Windows::UI::Xaml::Media::GradientBrush::ColorInterpolationModeProperty(); } },
          { L"gradientstops"s, []() { return winrt::Windows::UI::Xaml::Media::GradientBrush::GradientStopsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::GradientStop::ColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::GradientStop>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"color"s, []() { return winrt::Windows::UI::Xaml::Media::GradientStop::ColorProperty(); } },
          { L"offset"s, []() { return winrt::Windows::UI::Xaml::Media::GradientStop::OffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::ImageBrush::ImageSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::ImageBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"imagesource"s, []() { return winrt::Windows::UI::Xaml::Media::ImageBrush::ImageSourceProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::LineGeometry::StartPointProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::LineGeometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"startpoint"s, []() { return winrt::Windows::UI::Xaml::Media::LineGeometry::StartPointProperty(); } },
          { L"endpoint"s, []() { return winrt::Windows::UI::Xaml::Media::LineGeometry::EndPointProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::LineSegment::PointProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::LineSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"point"s, []() { return winrt::Windows::UI::Xaml::Media::LineSegment::PointProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::LinearGradientBrush::StartPointProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::LinearGradientBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"startpoint"s, []() { return winrt::Windows::UI::Xaml::Media::LinearGradientBrush::StartPointProperty(); } },
          { L"endpoint"s, []() { return winrt::Windows::UI::Xaml::Media::LinearGradientBrush::EndPointProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Matrix3DProjection::ProjectionMatrixProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Matrix3DProjection>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"projectionmatrix"s, []() { return winrt::Windows::UI::Xaml::Media::Matrix3DProjection::ProjectionMatrixProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::MatrixTransform::MatrixProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::MatrixTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"matrix"s, []() { return winrt::Windows::UI::Xaml::Media::MatrixTransform::MatrixProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::PathFigure::SegmentsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::PathFigure>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"segments"s, []() { return winrt::Windows::UI::Xaml::Media::PathFigure::SegmentsProperty(); } },
          { L"startpoint"s, []() { return winrt::Windows::UI::Xaml::Media::PathFigure::StartPointProperty(); } },
          { L"isclosed"s, []() { return winrt::Windows::UI::Xaml::Media::PathFigure::IsClosedProperty(); } },
          { L"isfilled"s, []() { return winrt::Windows::UI::Xaml::Media::PathFigure::IsFilledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::PathGeometry::FillRuleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::PathGeometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fillrule"s, []() { return winrt::Windows::UI::Xaml::Media::PathGeometry::FillRuleProperty(); } },
          { L"figures"s, []() { return winrt::Windows::UI::Xaml::Media::PathGeometry::FiguresProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::PlaneProjection::LocalOffsetXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::PlaneProjection>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"localoffsetx"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::LocalOffsetXProperty(); } },
          { L"localoffsety"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::LocalOffsetYProperty(); } },
          { L"localoffsetz"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::LocalOffsetZProperty(); } },
          { L"rotationx"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::RotationXProperty(); } },
          { L"rotationy"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::RotationYProperty(); } },
          { L"rotationz"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::RotationZProperty(); } },
          { L"centerofrotationx"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::CenterOfRotationXProperty(); } },
          { L"centerofrotationy"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::CenterOfRotationYProperty(); } },
          { L"centerofrotationz"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::CenterOfRotationZProperty(); } },
          { L"globaloffsetx"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::GlobalOffsetXProperty(); } },
          { L"globaloffsety"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::GlobalOffsetYProperty(); } },
          { L"globaloffsetz"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::GlobalOffsetZProperty(); } },
          { L"projectionmatrix"s, []() { return winrt::Windows::UI::Xaml::Media::PlaneProjection::ProjectionMatrixProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::PolyBezierSegment::PointsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::PolyBezierSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"points"s, []() { return winrt::Windows::UI::Xaml::Media::PolyBezierSegment::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::PolyLineSegment::PointsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::PolyLineSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"points"s, []() { return winrt::Windows::UI::Xaml::Media::PolyLineSegment::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::PolyQuadraticBezierSegment::PointsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::PolyQuadraticBezierSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"points"s, []() { return winrt::Windows::UI::Xaml::Media::PolyQuadraticBezierSegment::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::QuadraticBezierSegment::Point1Property())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::QuadraticBezierSegment>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"point1"s, []() { return winrt::Windows::UI::Xaml::Media::QuadraticBezierSegment::Point1Property(); } },
          { L"point2"s, []() { return winrt::Windows::UI::Xaml::Media::QuadraticBezierSegment::Point2Property(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::RectangleGeometry::RectProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::RectangleGeometry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"rect"s, []() { return winrt::Windows::UI::Xaml::Media::RectangleGeometry::RectProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::RevealBrush::ColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::RevealBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"color"s, []() { return winrt::Windows::UI::Xaml::Media::RevealBrush::ColorProperty(); } },
          { L"targettheme"s, []() { return winrt::Windows::UI::Xaml::Media::RevealBrush::TargetThemeProperty(); } },
          { L"alwaysusefallback"s, []() { return winrt::Windows::UI::Xaml::Media::RevealBrush::AlwaysUseFallbackProperty(); } },
          { L"state"s, []() { return winrt::Windows::UI::Xaml::Media::RevealBrush::StateProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::RotateTransform::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::RotateTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Windows::UI::Xaml::Media::RotateTransform::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Windows::UI::Xaml::Media::RotateTransform::CenterYProperty(); } },
          { L"angle"s, []() { return winrt::Windows::UI::Xaml::Media::RotateTransform::AngleProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::ScaleTransform::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::ScaleTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Windows::UI::Xaml::Media::ScaleTransform::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Windows::UI::Xaml::Media::ScaleTransform::CenterYProperty(); } },
          { L"scalex"s, []() { return winrt::Windows::UI::Xaml::Media::ScaleTransform::ScaleXProperty(); } },
          { L"scaley"s, []() { return winrt::Windows::UI::Xaml::Media::ScaleTransform::ScaleYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::SkewTransform::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::SkewTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Windows::UI::Xaml::Media::SkewTransform::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Windows::UI::Xaml::Media::SkewTransform::CenterYProperty(); } },
          { L"anglex"s, []() { return winrt::Windows::UI::Xaml::Media::SkewTransform::AngleXProperty(); } },
          { L"angley"s, []() { return winrt::Windows::UI::Xaml::Media::SkewTransform::AngleYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::SolidColorBrush::ColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::SolidColorBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"color"s, []() { return winrt::Windows::UI::Xaml::Media::SolidColorBrush::ColorProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::TileBrush::AlignmentXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::TileBrush>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"alignmentx"s, []() { return winrt::Windows::UI::Xaml::Media::TileBrush::AlignmentXProperty(); } },
          { L"alignmenty"s, []() { return winrt::Windows::UI::Xaml::Media::TileBrush::AlignmentYProperty(); } },
          { L"stretch"s, []() { return winrt::Windows::UI::Xaml::Media::TileBrush::StretchProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::TimelineMarker::TimeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::TimelineMarker>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"time"s, []() { return winrt::Windows::UI::Xaml::Media::TimelineMarker::TimeProperty(); } },
          { L"type"s, []() { return winrt::Windows::UI::Xaml::Media::TimelineMarker::TypeProperty(); } },
          { L"text"s, []() { return winrt::Windows::UI::Xaml::Media::TimelineMarker::TextProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::TransformGroup::ChildrenProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::TransformGroup>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"children"s, []() { return winrt::Windows::UI::Xaml::Media::TransformGroup::ChildrenProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::TranslateTransform::XProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::TranslateTransform>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"x"s, []() { return winrt::Windows::UI::Xaml::Media::TranslateTransform::XProperty(); } },
          { L"y"s, []() { return winrt::Windows::UI::Xaml::Media::TranslateTransform::YProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::XamlCompositionBrushBase::FallbackColorProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::XamlCompositionBrushBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fallbackcolor"s, []() { return winrt::Windows::UI::Xaml::Media::XamlCompositionBrushBase::FallbackColorProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Media.Animation.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::BackEase::AmplitudeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::BackEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"amplitude"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::BackEase::AmplitudeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::BeginStoryboard::StoryboardProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::BeginStoryboard>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"storyboard"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::BeginStoryboard::StoryboardProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::BounceEase::BouncesProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::BounceEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"bounces"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::BounceEase::BouncesProperty(); } },
          { L"bounciness"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::BounceEase::BouncinessProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::ColorAnimation::FromProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::ColorAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"from"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ColorAnimation::FromProperty(); } },
          { L"to"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ColorAnimation::ToProperty(); } },
          { L"by"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ColorAnimation::ByProperty(); } },
          { L"easingfunction"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ColorAnimation::EasingFunctionProperty(); } },
          { L"enabledependentanimation"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ColorAnimation::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::ColorAnimationUsingKeyFrames::EnableDependentAnimationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::ColorAnimationUsingKeyFrames>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabledependentanimation"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ColorAnimationUsingKeyFrames::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::ColorKeyFrame::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::ColorKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"value"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ColorKeyFrame::ValueProperty(); } },
          { L"keytime"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ColorKeyFrame::KeyTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::CommonNavigationTransitionInfo::IsStaggeringEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::CommonNavigationTransitionInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isstaggeringenabled"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::CommonNavigationTransitionInfo::IsStaggeringEnabledProperty(); } },
          { L"isstaggerelement"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::CommonNavigationTransitionInfo::IsStaggerElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::ContentThemeTransition::HorizontalOffsetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::ContentThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"horizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ContentThemeTransition::HorizontalOffsetProperty(); } },
          { L"verticaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ContentThemeTransition::VerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::ExitElementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"exitelement"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::ExitElementProperty(); } },
          { L"isentranceelement"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::IsEntranceElementProperty(); } },
          { L"isexitelement"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::IsExitElementProperty(); } },
          { L"exitelementcontainer"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ContinuumNavigationTransitionInfo::ExitElementContainerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimation::FromProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"from"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimation::FromProperty(); } },
          { L"to"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimation::ToProperty(); } },
          { L"by"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimation::ByProperty(); } },
          { L"easingfunction"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimation::EasingFunctionProperty(); } },
          { L"enabledependentanimation"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimation::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimationUsingKeyFrames::EnableDependentAnimationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimationUsingKeyFrames>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabledependentanimation"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DoubleAnimationUsingKeyFrames::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::DoubleKeyFrame::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::DoubleKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"value"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DoubleKeyFrame::ValueProperty(); } },
          { L"keytime"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DoubleKeyFrame::KeyTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::DragItemThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::DragItemThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DragItemThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::DragOverThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::DragOverThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DragOverThemeAnimation::TargetNameProperty(); } },
          { L"tooffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DragOverThemeAnimation::ToOffsetProperty(); } },
          { L"direction"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DragOverThemeAnimation::DirectionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::DrillInThemeAnimation::EntranceTargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::DrillInThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"entrancetargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DrillInThemeAnimation::EntranceTargetNameProperty(); } },
          { L"entrancetarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DrillInThemeAnimation::EntranceTargetProperty(); } },
          { L"exittargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DrillInThemeAnimation::ExitTargetNameProperty(); } },
          { L"exittarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DrillInThemeAnimation::ExitTargetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::DrillOutThemeAnimation::EntranceTargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::DrillOutThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"entrancetargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DrillOutThemeAnimation::EntranceTargetNameProperty(); } },
          { L"entrancetarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DrillOutThemeAnimation::EntranceTargetProperty(); } },
          { L"exittargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DrillOutThemeAnimation::ExitTargetNameProperty(); } },
          { L"exittarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DrillOutThemeAnimation::ExitTargetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::DropTargetItemThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::DropTargetItemThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::DropTargetItemThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::EasingColorKeyFrame::EasingFunctionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::EasingColorKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"easingfunction"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::EasingColorKeyFrame::EasingFunctionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::EasingDoubleKeyFrame::EasingFunctionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::EasingDoubleKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"easingfunction"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::EasingDoubleKeyFrame::EasingFunctionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::EasingFunctionBase::EasingModeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::EasingFunctionBase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"easingmode"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::EasingFunctionBase::EasingModeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::EasingPointKeyFrame::EasingFunctionProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::EasingPointKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"easingfunction"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::EasingPointKeyFrame::EasingFunctionProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::EdgeUIThemeTransition::EdgeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::EdgeUIThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"edge"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::EdgeUIThemeTransition::EdgeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::ElasticEase::OscillationsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::ElasticEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"oscillations"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ElasticEase::OscillationsProperty(); } },
          { L"springiness"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ElasticEase::SpringinessProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::EntranceNavigationTransitionInfo::IsTargetElementProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::EntranceNavigationTransitionInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"istargetelement"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::EntranceNavigationTransitionInfo::IsTargetElementProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::EntranceThemeTransition::FromHorizontalOffsetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::EntranceThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fromhorizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::EntranceThemeTransition::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::EntranceThemeTransition::FromVerticalOffsetProperty(); } },
          { L"isstaggeringenabled"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::EntranceThemeTransition::IsStaggeringEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::ExponentialEase::ExponentProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::ExponentialEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"exponent"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ExponentialEase::ExponentProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::FadeInThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::FadeInThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::FadeInThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::FadeOutThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::FadeOutThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::FadeOutThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::NavigationThemeTransition::DefaultNavigationTransitionInfoProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::NavigationThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"defaultnavigationtransitioninfo"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::NavigationThemeTransition::DefaultNavigationTransitionInfoProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::ObjectAnimationUsingKeyFrames::EnableDependentAnimationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::ObjectAnimationUsingKeyFrames>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabledependentanimation"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ObjectAnimationUsingKeyFrames::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::ObjectKeyFrame::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::ObjectKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"value"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ObjectKeyFrame::ValueProperty(); } },
          { L"keytime"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::ObjectKeyFrame::KeyTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PaneThemeTransition::EdgeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PaneThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"edge"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PaneThemeTransition::EdgeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PointAnimation::FromProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PointAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"from"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointAnimation::FromProperty(); } },
          { L"to"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointAnimation::ToProperty(); } },
          { L"by"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointAnimation::ByProperty(); } },
          { L"easingfunction"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointAnimation::EasingFunctionProperty(); } },
          { L"enabledependentanimation"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointAnimation::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PointAnimationUsingKeyFrames::EnableDependentAnimationProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PointAnimationUsingKeyFrames>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"enabledependentanimation"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointAnimationUsingKeyFrames::EnableDependentAnimationProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PointKeyFrame::ValueProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PointKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"value"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointKeyFrame::ValueProperty(); } },
          { L"keytime"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointKeyFrame::KeyTimeProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PointerDownThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PointerDownThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointerDownThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PointerUpThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PointerUpThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PointerUpThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PopInThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PopInThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PopInThemeAnimation::TargetNameProperty(); } },
          { L"fromhorizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PopInThemeAnimation::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PopInThemeAnimation::FromVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PopOutThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PopOutThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PopOutThemeAnimation::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PopupThemeTransition::FromHorizontalOffsetProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PopupThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fromhorizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PopupThemeTransition::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PopupThemeTransition::FromVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::PowerEase::PowerProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::PowerEase>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"power"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::PowerEase::PowerProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::RepositionThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::RepositionThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::RepositionThemeAnimation::TargetNameProperty(); } },
          { L"fromhorizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::RepositionThemeAnimation::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::RepositionThemeAnimation::FromVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::RepositionThemeTransition::IsStaggeringEnabledProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::RepositionThemeTransition>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"isstaggeringenabled"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::RepositionThemeTransition::IsStaggeringEnabledProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::SlideNavigationTransitionInfo::EffectProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::SlideNavigationTransitionInfo>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"effect"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SlideNavigationTransitionInfo::EffectProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::SplineColorKeyFrame::KeySplineProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::SplineColorKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"keyspline"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplineColorKeyFrame::KeySplineProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::SplineDoubleKeyFrame::KeySplineProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::SplineDoubleKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"keyspline"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplineDoubleKeyFrame::KeySplineProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::SplinePointKeyFrame::KeySplineProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::SplinePointKeyFrame>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"keyspline"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplinePointKeyFrame::KeySplineProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OpenedTargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"openedtargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OpenedTargetNameProperty(); } },
          { L"openedtarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OpenedTargetProperty(); } },
          { L"closedtargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ClosedTargetNameProperty(); } },
          { L"closedtarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ClosedTargetProperty(); } },
          { L"contenttargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ContentTargetNameProperty(); } },
          { L"contenttarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ContentTargetProperty(); } },
          { L"openedlength"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OpenedLengthProperty(); } },
          { L"closedlength"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ClosedLengthProperty(); } },
          { L"offsetfromcenter"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::OffsetFromCenterProperty(); } },
          { L"contenttranslationdirection"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ContentTranslationDirectionProperty(); } },
          { L"contenttranslationoffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitCloseThemeAnimation::ContentTranslationOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OpenedTargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"openedtargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OpenedTargetNameProperty(); } },
          { L"openedtarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OpenedTargetProperty(); } },
          { L"closedtargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ClosedTargetNameProperty(); } },
          { L"closedtarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ClosedTargetProperty(); } },
          { L"contenttargetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ContentTargetNameProperty(); } },
          { L"contenttarget"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ContentTargetProperty(); } },
          { L"openedlength"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OpenedLengthProperty(); } },
          { L"closedlength"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ClosedLengthProperty(); } },
          { L"offsetfromcenter"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::OffsetFromCenterProperty(); } },
          { L"contenttranslationdirection"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ContentTranslationDirectionProperty(); } },
          { L"contenttranslationoffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SplitOpenThemeAnimation::ContentTranslationOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::Storyboard::TargetPropertyProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::Storyboard>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetproperty"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::Storyboard::TargetPropertyProperty(); } },
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::Storyboard::TargetNameProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::SwipeBackThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::SwipeBackThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SwipeBackThemeAnimation::TargetNameProperty(); } },
          { L"fromhorizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SwipeBackThemeAnimation::FromHorizontalOffsetProperty(); } },
          { L"fromverticaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SwipeBackThemeAnimation::FromVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::SwipeHintThemeAnimation::TargetNameProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::SwipeHintThemeAnimation>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"targetname"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SwipeHintThemeAnimation::TargetNameProperty(); } },
          { L"tohorizontaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SwipeHintThemeAnimation::ToHorizontalOffsetProperty(); } },
          { L"toverticaloffset"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::SwipeHintThemeAnimation::ToVerticalOffsetProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Animation::Timeline::AutoReverseProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Animation::Timeline>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"autoreverse"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::Timeline::AutoReverseProperty(); } },
          { L"begintime"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::Timeline::BeginTimeProperty(); } },
          { L"duration"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::Timeline::DurationProperty(); } },
          { L"speedratio"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::Timeline::SpeedRatioProperty(); } },
          { L"fillbehavior"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::Timeline::FillBehaviorProperty(); } },
          { L"repeatbehavior"s, []() { return winrt::Windows::UI::Xaml::Media::Animation::Timeline::RepeatBehaviorProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Media.Imaging.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::CreateOptionsProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"createoptions"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::CreateOptionsProperty(); } },
          { L"urisource"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::UriSourceProperty(); } },
          { L"decodepixelwidth"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::DecodePixelWidthProperty(); } },
          { L"decodepixelheight"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::DecodePixelHeightProperty(); } },
          { L"decodepixeltype"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::DecodePixelTypeProperty(); } },
          { L"isanimatedbitmap"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::IsAnimatedBitmapProperty(); } },
          { L"isplaying"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::IsPlayingProperty(); } },
          { L"autoplay"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::AutoPlayProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Imaging::BitmapSource::PixelWidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Imaging::BitmapSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"pixelwidth"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapSource::PixelWidthProperty(); } },
          { L"pixelheight"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::BitmapSource::PixelHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Imaging::RenderTargetBitmap::PixelWidthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Imaging::RenderTargetBitmap>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"pixelwidth"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::RenderTargetBitmap::PixelWidthProperty(); } },
          { L"pixelheight"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::RenderTargetBitmap::PixelHeightProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Imaging::SvgImageSource::UriSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Imaging::SvgImageSource>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"urisource"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::SvgImageSource::UriSourceProperty(); } },
          { L"rasterizepixelwidth"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::SvgImageSource::RasterizePixelWidthProperty(); } },
          { L"rasterizepixelheight"s, []() { return winrt::Windows::UI::Xaml::Media::Imaging::SvgImageSource::RasterizePixelHeightProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Media.Media3D.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::CenterXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"centerx"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::CenterXProperty(); } },
          { L"centery"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::CenterYProperty(); } },
          { L"centerz"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::CenterZProperty(); } },
          { L"rotationx"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::RotationXProperty(); } },
          { L"rotationy"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::RotationYProperty(); } },
          { L"rotationz"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::RotationZProperty(); } },
          { L"scalex"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::ScaleXProperty(); } },
          { L"scaley"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::ScaleYProperty(); } },
          { L"scalez"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::ScaleZProperty(); } },
          { L"translatex"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::TranslateXProperty(); } },
          { L"translatey"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::TranslateYProperty(); } },
          { L"translatez"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::CompositeTransform3D::TranslateZProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Media::Media3D::PerspectiveTransform3D::DepthProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Media::Media3D::PerspectiveTransform3D>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"depth"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::PerspectiveTransform3D::DepthProperty(); } },
          { L"offsetx"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::PerspectiveTransform3D::OffsetXProperty(); } },
          { L"offsety"s, []() { return winrt::Windows::UI::Xaml::Media::Media3D::PerspectiveTransform3D::OffsetYProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Navigation.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Navigation::PageStackEntry::SourcePageTypeProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Navigation::PageStackEntry>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"sourcepagetype"s, []() { return winrt::Windows::UI::Xaml::Navigation::PageStackEntry::SourcePageTypeProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Printing.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Printing::PrintDocument::DocumentSourceProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Printing::PrintDocument>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"documentsource"s, []() { return winrt::Windows::UI::Xaml::Printing::PrintDocument::DocumentSourceProperty(); } },
        }
      }
    ));
  }
  // Windows.UI.Xaml.Shapes.h
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Shapes::Line::X1Property())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Shapes::Line>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"x1"s, []() { return winrt::Windows::UI::Xaml::Shapes::Line::X1Property(); } },
          { L"y1"s, []() { return winrt::Windows::UI::Xaml::Shapes::Line::Y1Property(); } },
          { L"x2"s, []() { return winrt::Windows::UI::Xaml::Shapes::Line::X2Property(); } },
          { L"y2"s, []() { return winrt::Windows::UI::Xaml::Shapes::Line::Y2Property(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Shapes::Path::DataProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Shapes::Path>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"data"s, []() { return winrt::Windows::UI::Xaml::Shapes::Path::DataProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Shapes::Polygon::FillRuleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Shapes::Polygon>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fillrule"s, []() { return winrt::Windows::UI::Xaml::Shapes::Polygon::FillRuleProperty(); } },
          { L"points"s, []() { return winrt::Windows::UI::Xaml::Shapes::Polygon::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Shapes::Polyline::FillRuleProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Shapes::Polyline>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fillrule"s, []() { return winrt::Windows::UI::Xaml::Shapes::Polyline::FillRuleProperty(); } },
          { L"points"s, []() { return winrt::Windows::UI::Xaml::Shapes::Polyline::PointsProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Shapes::Rectangle::RadiusXProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Shapes::Rectangle>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"radiusx"s, []() { return winrt::Windows::UI::Xaml::Shapes::Rectangle::RadiusXProperty(); } },
          { L"radiusy"s, []() { return winrt::Windows::UI::Xaml::Shapes::Rectangle::RadiusYProperty(); } },
        }
      }
    ));
  }
  if constexpr (std::is_same_v<winrt::Microsoft::UI::Xaml::DependencyProperty, decltype(winrt::Windows::UI::Xaml::Shapes::Shape::FillProperty())>)
  {
    result.insert(std::make_pair(winrt::guid_of<winrt::Windows::UI::Xaml::Shapes::Shape>(),
      ObjectData {
        ObjectType::Library, 
        {
          { L"fill"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::FillProperty(); } },
          { L"stroke"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StrokeProperty(); } },
          { L"strokemiterlimit"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StrokeMiterLimitProperty(); } },
          { L"strokethickness"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StrokeThicknessProperty(); } },
          { L"strokestartlinecap"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StrokeStartLineCapProperty(); } },
          { L"strokeendlinecap"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StrokeEndLineCapProperty(); } },
          { L"strokelinejoin"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StrokeLineJoinProperty(); } },
          { L"strokedashoffset"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StrokeDashOffsetProperty(); } },
          { L"strokedashcap"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StrokeDashCapProperty(); } },
          { L"strokedasharray"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StrokeDashArrayProperty(); } },
          { L"stretch"s, []() { return winrt::Windows::UI::Xaml::Shapes::Shape::StretchProperty(); } },
        }
      }
    ));
  }
  return result;
}();
