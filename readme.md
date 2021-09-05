# Dracutio Keymap for OLED screens
> This repo was forked from the official QMK github and has been modified by me. Follow at your own risk, since I'm not a professional at QMK

## Steps

### 1. Firmware
1. Install and setup [QMK](https://beta.docs.qmk.fm/tutorial/newbs_getting_started)
2. Go to ``qmk/keyboards/<your_keyboard_vendor>/<keyboard_name>/keymaps>`` and create a new Folder called ``dracutio``
3. Copy code from this commit [39b2f533afc6cc95e39a8797a99681f01f847e19](https://github.com/vuon0029/qmk/commit/39b2f533afc6cc95e39a8797a99681f01f847e19) with all files and folders OR find all the files [here](https://github.com/vuon0029/qmk/tree/master/keyboards/mechwild/mercutio/keymaps/dracutio)
4. Compile with **QMK** ``qmk compile -kb <your_keyboard_vendor>/<keyboard_name> -km dracutio`` and flash keyboard with [QMK Toolbox](https://github.com/qmk/qmk_toolbox)
 
 Now you should be able to see keyboard typing matrix and layers
 
 ### 2. HID Display
 1. Clone or download code from [QMK_OLED_HID](https://github.com/vuon0029/qmk_oled_hid)
 2. ``cd`` into folder and run ``npm install`` to install all dependencies
 3. Replace **Keyboard info**, you should see message "**Keyboard connection established.**" and **HID** on OLED screen if done correctly
 <img width="483" alt="Screen Shot 2021-09-05 at 4 00 35 PM" src="https://user-images.githubusercontent.com/55266028/132139945-e2701e7f-1e43-4e05-bb00-ba589db0d958.png">

 4. Run ``node index.js`` or ``sudo node index.js`` if it doesn't have permission

At this point, there should be messages showing on NodeJS server console when navigating between layers. By default, **PC Stats** will show on layer 2 and **Date & Time** will show on layer 3


