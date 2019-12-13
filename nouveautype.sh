#!/bin/bash


petit_nom=potard

DOAP_NAME=Plujain-${petit_nom^}
NAME=plujain-$petit_nom
PLUGIN=$petit_nom.so
URL=http://plujain/plugins/$petit_nom


cd $(dirname "$0")

sed -i Makefile -e "s/NAME = .*$/NAME = $NAME/" -e "s/PLUGIN = .*$/PLUGIN = $PLUGIN/"

manifest="@prefix lv2: <http://lv2plug.in/ns/lv2core#>.
@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#>.

<$URL> a lv2:Plugin;
    lv2:binary <$PLUGIN> ;
    rdfs:seeAlso <$petit_nom.ttl>."
    
echo "$manifest" > ttl/manifest.ttl

mv ttl/type.ttl ttl/$petit_nom.ttl
sed -i ttl/$petit_nom.ttl -e "s|^<http://.*>$|<$URL>|"  -e "s/doap:name .*$/doap:name \"$DOAP_NAME\";/"

mv src/type.cpp src/$petit_nom.cpp
sed -i src/$petit_nom.cpp -e "s|#define PLUGIN_URI .*$|#define PLUGIN_URI \"$URL\"|"
