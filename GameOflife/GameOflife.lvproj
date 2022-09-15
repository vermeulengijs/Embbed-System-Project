<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="13008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="GameOflife.vi" Type="VI" URL="../GameOflife.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Space Constant.vi" Type="VI" URL="/&lt;vilib&gt;/dlg_ctls.llb/Space Constant.vi"/>
				<Item Name="VISA Configure Serial Port" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port"/>
				<Item Name="VISA Configure Serial Port (Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Instr).vi"/>
				<Item Name="VISA Configure Serial Port (Serial Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Serial Instr).vi"/>
			</Item>
			<Item Name="GetCoordinates.vi" Type="VI" URL="../GetCoordinates.vi"/>
			<Item Name="Getevrything.vi" Type="VI" URL="../Getevrything.vi"/>
			<Item Name="LFTOF.vi" Type="VI" URL="../LFTOF.vi"/>
			<Item Name="NEW_TOF_TO_NEW_LF.vi" Type="VI" URL="../NEW_TOF_TO_NEW_LF.vi"/>
			<Item Name="SchrijfLijn.vi" Type="VI" URL="../SchrijfLijn.vi"/>
			<Item Name="SubDeadOrAllive.vi" Type="VI" URL="../SubDeadOrAllive.vi"/>
			<Item Name="Text To boolean.vi" Type="VI" URL="../Text To boolean.vi"/>
			<Item Name="Values of neighbours to bools.vi" Type="VI" URL="../Values of neighbours to bools.vi"/>
			<Item Name="waardes neighbours.vi" Type="VI" URL="../waardes neighbours.vi"/>
			<Item Name="Wrap coordiantes off neighbours.vi" Type="VI" URL="../Wrap coordiantes off neighbours.vi"/>
			<Item Name="Wrap values off neighbours.vi" Type="VI" URL="../Wrap values off neighbours.vi"/>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="GameOflife" Type="EXE">
				<Property Name="App_copyErrors" Type="Bool">true</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{502FF936-FE70-4EDA-B38A-410F30B54A47}</Property>
				<Property Name="App_INI_GUID" Type="Str">{73F06AEB-2024-4057-B112-ABBD159ACA24}</Property>
				<Property Name="App_serverConfig.httpPort" Type="Int">8002</Property>
				<Property Name="Bld_autoIncrement" Type="Bool">true</Property>
				<Property Name="Bld_buildCacheID" Type="Str">{F04E0138-461B-4B4C-8F79-EE2CACEDA04D}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">GameOflife</Property>
				<Property Name="Bld_excludeInlineSubVIs" Type="Bool">true</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">../builds/NI_AB_PROJECTNAME/GameOflife</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{146D643D-81FE-4153-90CC-93336FA5F949}</Property>
				<Property Name="Bld_version.build" Type="Int">2</Property>
				<Property Name="Bld_version.major" Type="Int">1</Property>
				<Property Name="Destination[0].destName" Type="Str">Application.exe</Property>
				<Property Name="Destination[0].path" Type="Path">../builds/NI_AB_PROJECTNAME/GameOflife/Application.exe</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../builds/NI_AB_PROJECTNAME/GameOflife/data</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Source[0].itemID" Type="Str">{E28DEC68-E897-476A-B54C-7DFD3CCDFA84}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/GameOflife.vi</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="SourceCount" Type="Int">2</Property>
				<Property Name="TgtF_fileDescription" Type="Str">GameOflife</Property>
				<Property Name="TgtF_internalName" Type="Str">GameOflife</Property>
				<Property Name="TgtF_legalCopyright" Type="Str">Copyright © 2015 </Property>
				<Property Name="TgtF_productName" Type="Str">GameOflife</Property>
				<Property Name="TgtF_targetfileGUID" Type="Str">{41198CBA-0D14-4180-8097-947EE071E795}</Property>
				<Property Name="TgtF_targetfileName" Type="Str">Application.exe</Property>
			</Item>
		</Item>
	</Item>
</Project>
