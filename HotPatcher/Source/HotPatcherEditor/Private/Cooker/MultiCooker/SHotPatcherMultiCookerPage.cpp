// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

// #include "HotPatcherPrivatePCH.h"
#include "SHotPatcherMultiCookerPage.h"
#include "FlibHotPatcherEditorHelper.h"
#include "FLibAssetManageHelperEx.h"
#include "HotPatcherLog.h"
#include "HotPatcherEditor.h"
#include "FMultiCookerSettings.h"
// engine header
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Layout/SSeparator.h"
#include "HAL/FileManager.h"
#include "Kismet/KismetTextLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopedSlowTask.h"

#define LOCTEXT_NAMESPACE "SHotPatcherMultiCookerPage"

void SHotPatcherMultiCookerPage::Construct(const FArguments& InArgs, TSharedPtr<FHotPatcherCookerModel> InCreatePatchModel)
{
	CookerSettings = MakeShareable(new FMultiCookerSettings);
	CreateExportFilterListView();

	mCreatePatchModel = InCreatePatchModel;

	ChildSlot
		[

			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SettingsView->GetWidget()->AsShared()
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0, 8.0, 0.0, 0.0)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.Padding(4, 4, 10, 4)
			[
				SNew(SButton)
				.Text(LOCTEXT("CookContent", "Cook Content"))
				.OnClicked(this,&SHotPatcherMultiCookerPage::RunCook)
				.IsEnabled(this,&SHotPatcherMultiCookerPage::CanCook)
				.ToolTipText(this,&SHotPatcherMultiCookerPage::GetGenerateTooltipText)
			]
		];
}

void SHotPatcherMultiCookerPage::ImportConfig()
{
	UE_LOG(LogHotPatcher, Log, TEXT("Release Import Config"));
	TArray<FString> Files = this->OpenFileDialog();
	if (!Files.Num()) return;

	FString LoadFile = Files[0];

	FString JsonContent;
	if (UFLibAssetManageHelperEx::LoadFileToString(LoadFile, JsonContent))
	{
		// UFlibHotPatcherEditorHelper::DeserializeReleaseConfig(ExportReleaseSettings, JsonContent);
		UFlibPatchParserHelper::TDeserializeJsonStringAsStruct(JsonContent,*CookerSettings);
		SettingsView->GetDetailsView()->ForceRefresh();
	}
}

void SHotPatcherMultiCookerPage::ExportConfig()const
{
	UE_LOG(LogHotPatcher, Log, TEXT("Release Export Config"));
	TArray<FString> Files = this->SaveFileDialog();

	if (!Files.Num()) return;

	FString SaveToFile = Files[0].EndsWith(TEXT(".json")) ? Files[0] : Files[0].Append(TEXT(".json"));

	if (CookerSettings)
	{

		FString SerializedJsonStr;
		UFlibPatchParserHelper::TSerializeStructAsJsonString(*CookerSettings,SerializedJsonStr);
		if (FFileHelper::SaveStringToFile(SerializedJsonStr, *SaveToFile))
		{
			FText Msg = LOCTEXT("SavedPatchConfigMas", "Successd to Export the Patch Config.");
			UFlibHotPatcherEditorHelper::CreateSaveFileNotify(Msg, SaveToFile);
		}
	}
}

void SHotPatcherMultiCookerPage::ResetConfig()
{
	UE_LOG(LogHotPatcher, Log, TEXT("Release Clear Config"));
	FString DefaultSettingJson;
	UFlibPatchParserHelper::TSerializeStructAsJsonString(*FExportReleaseSettings::Get(),DefaultSettingJson);
	UFlibPatchParserHelper::TDeserializeJsonStringAsStruct(DefaultSettingJson,*CookerSettings);
	SettingsView->GetDetailsView()->ForceRefresh();
}
void SHotPatcherMultiCookerPage::DoGenerate()
{
	UE_LOG(LogHotPatcher, Log, TEXT("Release DoGenerate"));
}

void SHotPatcherMultiCookerPage::CreateExportFilterListView()
{
	// Create a property view
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bAllowSearch = true;
		DetailsViewArgs.bHideSelectionTip = true;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bSearchInitialKeyFocus = true;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.NotifyHook = nullptr;
		DetailsViewArgs.bShowOptions = true;
		DetailsViewArgs.bShowModifiedPropertiesOption = false;
		DetailsViewArgs.bShowScrollBar = false;
		DetailsViewArgs.bShowOptions = true;
		DetailsViewArgs.bUpdatesFromSelection= true;
	}

	FStructureDetailsViewArgs StructureViewArgs;
	{
		StructureViewArgs.bShowObjects = true;
		StructureViewArgs.bShowAssets = true;
		StructureViewArgs.bShowClasses = true;
		StructureViewArgs.bShowInterfaces = true;
	}

	SettingsView = EditModule.CreateStructureDetailView(DetailsViewArgs, StructureViewArgs, nullptr);
	FStructOnScope* Struct = new FStructOnScope(FMultiCookerSettings::StaticStruct(), (uint8*)CookerSettings.Get());
	// SettingsView->GetOnFinishedChangingPropertiesDelegate().AddRaw(CookerSettings.Get(),&FExportReleaseSettings::OnFinishedChangingProperties);
	// SettingsView->GetDetailsView()->RegisterInstancedCustomPropertyLayout(FExportReleaseSettings::StaticStruct(),FOnGetDetailCustomizationInstance::CreateStatic(&FMultiCookerSettings::MakeInstance));
	SettingsView->SetStructureData(MakeShareable(Struct));
}

bool SHotPatcherMultiCookerPage::CanCook()const
{
	bool bCanExport=false;
	return bCanExport;
}

FReply SHotPatcherMultiCookerPage::RunCook()
{
	
	return FReply::Handled();
}

FText SHotPatcherMultiCookerPage::GetGenerateTooltipText() const
{
	FString FinalString;
	return UKismetTextLibrary::Conv_StringToText(FinalString);
}

#undef LOCTEXT_NAMESPACE
