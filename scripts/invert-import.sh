#!/usr/bin/env bash

cleanup() {
  rm "$1"*.png
}

invert_pngs() {
  for file in $(ls "$1"*.png); do invert "$file" $2 $3 $4; done
}

make_gif() {
  n=$(ls "$1"*.png | grep -e "$1"-[[:digit:]]*\.png$ | wc -l)
  convert $(for ((i=0; i<$n; i++)); do printf -- "-delay 5 "$1"-%s.png-out.png " $i; done;) "$1"-out.gif
}

make_avi() {
    avconv -i "$1"-%d.png-out.png  "$1"-out.avi
}

_make_avi_le_quality() {
    avconv -loop 1 -i "$1"-%d.png -f avi -me_method full -c:v libx264 -c:a n -b 512k \
      -qcomp 0.5 -r 50  -crf 20 "$1".avi
}

make_pngs() {
  convert -coalesce "$1".gif "$1".png
}
