# Embedded Systems I - Helicopter rig 
 Members: Gabe Lake, Thavy Thach, Jed Mcdermott

### Creating CCS projects

* [1] Right-Click Active project and Click "Properties".
* [2] Go To Build > ARM Compiler > Include Options. 
* [3] Here, click the the + button or pencil button in the "Add dir to #include search path" to specify the "locationX\TivaWare_C_Series-2.1.4.178"
* [4] In the same place, click the + button or pencil button in the "Add dir to include search path" to specify the "locationY\newly_created_ccs_project"
* [5] Go to Build > Arm Linker > File Search Path.
* [6] Here, click the + button or pencil button in the "Include lirary file or command file as input" to specify the "locationZ\Tiva_C_Series-2.1.4.178\driverlib\ccs\Debug\driverlib.lib"
* [7] Click Ok
* [8] Successfully created a new project.

### Adding Folders to Existing CSS Projects
* [1] Right click Active project and navigate to New > Folder.
* [2] Go the bottom and expand advanced.
* [3] Click "Link to alternate location" option.
* [4] Click "Browse" and find desired folder.
* [5] Folder is now linked and is a dependency of other files.

### Adding Files to Existing CSS Projects
* [1] Right click Activate proect and navigate to New > File.
* [2] Go the bottom and expand advanced.
* [3] Click "link to file in the file system"
* [4] Click Browse and locate desired file.
* [5] File is now linked and is a dependency of other files.

