import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning

Rectangle {
    id: mapContainer
    width: parent ? parent.width : 400
    height: parent ? parent.height : 300

    property string currentAddress: ""
    property real currentLat: 36.8065
    property real currentLon: 10.1815
    property bool gpsEnabled: false

    signal addressUpdated(string address, real lat, real lon)

    Plugin {
        id: mapPlugin
        name: "osm"
        // Parameters to bypass Thunderforest API Key and use standard OSM:
        PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: "true" }
        PluginParameter { name: "osm.mapping.custom.host"; value: "https://tile.openstreetmap.org/" }
    }

    Loader {
        id: gpsSourceLoader
        active: mapContainer.gpsEnabled
        sourceComponent: PositionSource {
            updateInterval: 1000
            onPositionChanged: {
                if (valid) {
                    map.center = position.coordinate;
                    marker.coordinate = position.coordinate;
                    if (mapContainer.currentAddress === "") {
                        mapContainer.currentAddress = marker.coordinate.latitude.toFixed(5) + ", " + marker.coordinate.longitude.toFixed(5);
                    }
                    active = false;
                }
            }
        }
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(36.8065, 10.1815)
        zoomLevel: 16 // Increased default zoom to clearly show buildings and hospitals
        
        Component.onCompleted: {
            // Activate the custom URL map type to avoid API key require layer
            for (var i = 0; i < supportedMapTypes.length; i++) {
                if (supportedMapTypes[i].name === "Custom URL Map") {
                    activeMapType = supportedMapTypes[i];
                }
            }
        }

        DragHandler {
            id: drag
            target: null
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
        }

        WheelHandler {
            id: wheel
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            rotationScale: 1/120
            property: "zoomLevel"
            target: map
        }

        MapQuickItem {
            id: marker
            coordinate: map.center
            anchorPoint.x: 12
            anchorPoint.y: 24
            sourceItem: Rectangle {
                width: 24
                height: 24
                color: "transparent"
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    color: "red"
                    border.color: "white"
                    border.width: 2
                }
            }
        }

        TapHandler {
            onTapped: (eventPoint) => {
                // Ensure we don't place a pin if the user clicked over the controls
                if (typeof controlsColumn !== "undefined") {
                    var p = map.mapToItem(controlsColumn, eventPoint.position.x, eventPoint.position.y);
                    if (p.x >= 0 && p.x <= controlsColumn.width && p.y >= 0 && p.y <= controlsColumn.height) return;
                }
                if (typeof searchRect !== "undefined") {
                    var p2 = map.mapToItem(searchRect, eventPoint.position.x, eventPoint.position.y);
                    if (p2.x >= 0 && p2.x <= searchRect.width && p2.y >= 0 && p2.y <= searchRect.height) return;
                }
                
                marker.coordinate = map.toCoordinate(eventPoint.position);
                mapContainer.currentAddress = marker.coordinate.latitude.toFixed(5) + ", " + marker.coordinate.longitude.toFixed(5);
                mapContainer.currentLat = marker.coordinate.latitude;
                mapContainer.currentLon = marker.coordinate.longitude;
            }
        }
    } // End Map

    // On-screen map controls
    Column {
        id: controlsColumn
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 15
        spacing: 10
        
        Rectangle {
            width: 40; height: 40
            radius: 20
            color: "white"
            border.color: "#ccc"
            Text { anchors.centerIn: parent; text: "🎯"; font.pixelSize: 18 }
            TapHandler { 
                onTapped: {
                    var ps = gpsSourceLoader.item;
                    if (mapContainer.gpsEnabled && ps) {
                        ps.active = true;
                        ps.update();
                    }
                    if (mapContainer.gpsEnabled && ps && ps.valid && ps.position.coordinate.isValid) {
                        map.center = ps.position.coordinate;
                        marker.coordinate = ps.position.coordinate;
                        mapContainer.currentAddress = marker.coordinate.latitude.toFixed(5) + ", " + marker.coordinate.longitude.toFixed(5);
                        mapContainer.currentLat = marker.coordinate.latitude;
                        mapContainer.currentLon = marker.coordinate.longitude;
                    } else {
                        // Fallback for Desktop PCs without hardware GPS location drivers
                        var xhr = new XMLHttpRequest();
                        xhr.open("GET", "http://ip-api.com/json/", true);
                        xhr.onreadystatechange = function() {
                            if (xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200) {
                                var response = JSON.parse(xhr.responseText);
                                if (response.status === "success") {
                                    var coord = QtPositioning.coordinate(response.lat, response.lon);
                                    map.center = coord;
                                    marker.coordinate = coord;
                                    mapContainer.currentAddress = coord.latitude.toFixed(5) + ", " + coord.longitude.toFixed(5);
                                    mapContainer.currentLat = coord.latitude;
                                    mapContainer.currentLon = coord.longitude;
                                }
                            }
                        }
                        xhr.send();
                    }
                }
            }
        }
        Rectangle {
            width: 40; height: 40
            radius: 20
            color: "white"
            border.color: "#ccc"
            Text { anchors.centerIn: parent; text: "+"; font.pixelSize: 24; font.bold: true; color: "#333" }
            TapHandler { onTapped: map.zoomLevel += 1 }
        }
        Rectangle {
            width: 40; height: 40
            radius: 20
            color: "white"
            border.color: "#ccc"
            Text { anchors.centerIn: parent; text: "-"; font.pixelSize: 24; font.bold: true; color: "#333" }
            TapHandler { onTapped: map.zoomLevel -= 1 }
        }
    }

    Rectangle {
        id: searchRect
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.9
        height: 40
        anchors.margins: 10
        radius: 5
        color: "white"
        border.color: "#ccc"
        
        TextInput {
            id: searchInput
            anchors.fill: parent
            anchors.margins: 8
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 14
            text: "Rechercher une adresse..."
            color: "gray"
            onFocusChanged: {
                if (focus && text === "Rechercher une adresse...") {
                    text = "";
                    color = "black";
                } else if (!focus && text === "") {
                    text = "Rechercher une adresse...";
                    color = "gray";
                }
            }
            onAccepted: {
                geocodeModel.query = searchInput.text;
                geocodeModel.update();
            }
        }
    }

    GeocodeModel {
        id: geocodeModel
        plugin: mapPlugin
        onLocationsChanged: {
            if (count > 0) {
                var loc = get(0);
                map.center = loc.coordinate;
                marker.coordinate = loc.coordinate;
                mapContainer.currentAddress = loc.coordinate.latitude.toFixed(5) + ", " + loc.coordinate.longitude.toFixed(5);
                mapContainer.currentLat = loc.coordinate.latitude;
                mapContainer.currentLon = loc.coordinate.longitude;
            }
        }
    }

    function setInitialLocation(lat, lon, addressStr) {
        var coord = QtPositioning.coordinate(lat, lon);
        
        if (addressStr && addressStr.length > 0 && addressStr !== "---") {
            var parts = addressStr.split(",");
            if (parts.length === 2) {
                var pLat = parseFloat(parts[0].trim());
                var pLon = parseFloat(parts[1].trim());
                if (!isNaN(pLat) && !isNaN(pLon)) {
                    coord = QtPositioning.coordinate(pLat, pLon);
                }
            }
            mapContainer.currentAddress = addressStr;
        } else {
            mapContainer.currentAddress = coord.latitude.toFixed(5) + ", " + coord.longitude.toFixed(5);
        }
        
        map.center = coord;
        marker.coordinate = coord;
    }
}
