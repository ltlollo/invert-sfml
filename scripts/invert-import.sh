#!/usr/bin/env bash

cleanup() {
  rm "$1"*.png
}

invert_pngs() {
  if [[ $2 && $3 && $4 ]]; then
      for file in $(ls "$1"*.png); do transform-img -i "$file" -c$2,$3,$4; done
  else
    for file in $(ls "$1"*.png); do transform-img -i "$file"; done
  fi
}

make_gif() {
  n=$(ls "$1"*.png | grep -e "$1"-[[:digit:]]*\.png$ | wc -l)
  if [[ $n -eq 0 ]]; then
    convert "$1".png "$1"-out.gif
  else
    convert $(for ((i=0; i<$n; i++)); do printf -- "-delay 5 "$1"-%s.png-out.png " $i; done;) "$1"-out.gif
  fi
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
