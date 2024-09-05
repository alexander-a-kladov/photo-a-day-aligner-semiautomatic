# photo-a-day-aligner-semiautomatic
This applictation allows to align stack of photos in semiautomatic fashion, that is to manually draw line that is would be the same on each photo, after doing this for each image automatically generate aligned images

./imagesAligner images.txt

or

./imagesAligner images.txt <digit> [crop_file.txt]

where <digit> is number of picture to show on load, and [crop_file.txt] file with crop params default is crop_images.txt

file images.txt must contain in first line path to images directory, and if where is no more data all images will be loaded from that directory,

after saving reference point it will contain also in each next line 'image_name x y x2 y2', that is file name of the picture and coordiantes of the left and right alignment point

