import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning

Rectangle {
    id: mapContainer
    width: parent ? parent.width : 400
    height: parent ? parent.height : 300

    property string currentAddress: ""
    property real currentLat: 0.0
    property real currentLon: 0.0
    property real startLat: 0.0
    property real startLon: 0.0
    property bool gpsEnabled: true
    property bool isInitialSet: false
    property string pathHistory: ""
    property bool showHistory: false
    property bool showOptimalRoute: true
    property bool isReadOnly: false
    property bool showLocButton: true

    signal addressUpdated(string address, real lat, real lon)

    function drawHistoryPolyline(historyStr) {
        historyPolyline.path = [];
        if (!historyStr || historyStr === "") return;
        var points = historyStr.split(";");
        var newPath = [];
        for (var i = 0; i < points.length; i++) {
            var coords = points[i].split(",");
            if (coords.length === 2) {
                var pLat = parseFloat(coords[0].trim());
                var pLon = parseFloat(coords[1].trim());
                if (!isNaN(pLat) && !isNaN(pLon)) {
                    newPath.push(QtPositioning.coordinate(pLat, pLon));
                }
            }
        }
        historyPolyline.path = newPath;
    }

    RouteModel {
        id: routeModel
        plugin: mapPlugin
        query: RouteQuery {
            id: routeQuery
        }
    }

    function updateRoute() {
        routeQuery.clearWaypoints();
        routeQuery.addWaypoint(startMarker.coordinate);
        routeQuery.addWaypoint(marker.coordinate);
        routeModel.update();
    }

    function fetchCurrentLocationAndCenter() {
        var ps = gpsSourceLoader.item;
        if (mapContainer.gpsEnabled && ps) {
            ps.active = true;
            ps.update();
        } else {
            console.log("GPS Location unavailable. Cannot fetch precise location automatically.");
        }
    }

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
            active: mapContainer.gpsEnabled
            preferredPositioningMethods: PositionSource.AllPositioningMethods
            onPositionChanged: {
                if (valid && position.coordinate.isValid) {
                    var coord = position.coordinate;
                    // Only update if we get a non-zero coordinate or if we were at 0,0
                    if (coord.latitude !== 0 || coord.longitude !== 0) {
                        map.center = coord;
                        startMarker.coordinate = coord;
                        mapContainer.startLat = coord.latitude;
                        mapContainer.startLon = coord.longitude;
                        
                        if (!mapContainer.isInitialSet && mapContainer.currentAddress === "") {
                            marker.coordinate = coord;
                            mapContainer.currentLat = coord.latitude;
                            mapContainer.currentLon = coord.longitude;
                            mapContainer.currentAddress = coord.latitude.toFixed(5) + ", " + coord.longitude.toFixed(5);
                        }
                        updateRoute();
                    }
                }
            }
        }
    }

    Timer {
        id: retryLocateTimer
        interval: 2000
        running: mapContainer.gpsEnabled && (mapContainer.startLat === 0.0) && !mapContainer.isInitialSet
        repeat: true
        onTriggered: fetchCurrentLocationAndCenter()
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(mapContainer.startLat, mapContainer.startLon)
        zoomLevel: 16 // Increased default zoom to clearly show buildings and hospitals
        
        Component.onCompleted: {
            // Activate the custom URL map type to avoid API key require layer
            for (var i = 0; i < supportedMapTypes.length; i++) {
                if (supportedMapTypes[i].name === "Custom URL Map") {
                    activeMapType = supportedMapTypes[i];
                }
            }
            
            // If GPS is enabled and it's not a pre-set order, start finding location immediately
            if (mapContainer.gpsEnabled && !mapContainer.isInitialSet) {
                 fetchCurrentLocationAndCenter();
            }
        }

        MapPolyline {
            id: historyPolyline
            visible: mapContainer.showHistory
            line.color: "#F97316" // Orange color for historical IoT route
            line.width: 4
            opacity: 0.9
            smooth: true
        }

        MapItemView {
            model: routeModel
            visible: mapContainer.showOptimalRoute
            delegate: MapRoute {
                route: routeData
                line.color: "#3B82F6"
                line.width: 5
                smooth: true
                opacity: 0.8
            }
        }

        MapQuickItem {
            id: startMarker
            coordinate: QtPositioning.coordinate(mapContainer.startLat, mapContainer.startLon)
            anchorPoint.x: 12
            anchorPoint.y: 12
            sourceItem: Rectangle {
                width: 24
                height: 24
                radius: 12
                color: "#EF4444" // Red dot for Start (User request)
                border.color: "white"
                border.width: 2
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
                    color: "#3B82F6" // Blue pin for Destination (User request)
                    border.color: "white"
                    border.width: 2
                }
            }
        }

        MapQuickItem {
            id: phoneMarker
            coordinate: QtPositioning.coordinate(0, 0)
            anchorPoint.x: 12
            anchorPoint.y: 12
            visible: false
            sourceItem: Rectangle {
                width: 24
                height: 24
                radius: 12
                color: "#10B981" // Green dot for Phone Real-time Location
                border.color: "white"
                border.width: 2

                // Pulsing animation
                SequentialAnimation on scale {
                    loops: Animation.Infinite
                    PropertyAnimation { to: 1.2; duration: 800; easing.type: Easing.InOutQuad }
                    PropertyAnimation { to: 1.0; duration: 800; easing.type: Easing.InOutQuad }
                }
            }
        }

        TapHandler {
            enabled: !mapContainer.isReadOnly
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
                updateRoute();
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
            visible: mapContainer.showLocButton
            radius: 20
            color: 'white'
            border.color: '#ccc'
            Text { anchors.centerIn: parent; text: 'LOC'; font.pixelSize: 12; font.bold: true; color: '#007AFF' }
            TapHandler {
                onTapped: {
                    console.log("LOC button tapped! Attempting bridge...");
                    if (typeof mainWindow !== 'undefined' && mainWindow !== null) {
                        mainWindow.fetchDeviceLocation(mapContainer);
                    } else {
                        console.log("mainWindow bridge not available, falling back to QtPositioning");
                        fetchCurrentLocationAndCenter();
                    }
                }
            }
        }
        Rectangle {
            width: 40; height: 40
            radius: 20
            color: 'white'
            border.color: '#ccc'
            Text { anchors.centerIn: parent; text: '+'; font.pixelSize: 24; font.bold: true; color: '#333' }
            TapHandler { onTapped: map.zoomLevel += 1 }
        }
        Rectangle {
            width: 40; height: 40
            radius: 20
            color: 'white'
            border.color: '#ccc'
            Text { anchors.centerIn: parent; text: '-'; font.pixelSize: 24; font.bold: true; color: '#333' }
            TapHandler { onTapped: map.zoomLevel -= 1 }
        }
    }

    Rectangle {
        id: searchRect
        visible: !mapContainer.isReadOnly
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
                updateRoute();
            }
        }
    }
    
    function setPathHistory(historyStr) {
        mapContainer.pathHistory = historyStr;
        drawHistoryPolyline(historyStr);
    }
    
    function resetMap() {
        mapContainer.isInitialSet = false;
        mapContainer.currentLat = 0;
        mapContainer.currentLon = 0;
        mapContainer.startLat = 0;
        mapContainer.startLon = 0;
        mapContainer.currentAddress = "";
        marker.coordinate = QtPositioning.coordinate(0,0);
        startMarker.coordinate = QtPositioning.coordinate(0,0);
        routeQuery.clearWaypoints();
        routeModel.update();
        
        // Clear history
        mapContainer.pathHistory = "";
        pathPolyline.path = [];
        
        // Reset view to Tunis default
        map.center = QtPositioning.coordinate(36.8065, 10.1815);
        map.zoomLevel = 12;
    }
    
    function setInitialLocation(sLat, sLon, addressStr) {
        mapContainer.isInitialSet = true;
        
        // Set Start location
        var startCoord = QtPositioning.coordinate(sLat, sLon);
        startMarker.coordinate = startCoord;
        mapContainer.startLat = sLat;
        mapContainer.startLon = sLon;
        
        // Set Destination location (defaulting to start if no addressStr)
        var destCoord = startCoord;
        
        if (addressStr && addressStr.length > 0 && addressStr !== "---") {
            var parts = addressStr.split(",");
            if (parts.length === 2) {
                var pLat = parseFloat(parts[0].trim());
                var pLon = parseFloat(parts[1].trim());
                if (!isNaN(pLat) && !isNaN(pLon)) {
                    destCoord = QtPositioning.coordinate(pLat, pLon);
                }
            }
            mapContainer.currentAddress = addressStr;
        } else {
            mapContainer.currentAddress = destCoord.latitude.toFixed(5) + ", " + destCoord.longitude.toFixed(5);
        }
        
        map.center = destCoord;
        marker.coordinate = destCoord;
        mapContainer.currentLat = destCoord.latitude;
        mapContainer.currentLon = destCoord.longitude;
        
        // Auto-locate only if start is missing AND it's an active delivery (not a historical trip)
        if (sLat === 0 && sLon === 0 && mapContainer.showOptimalRoute) {
            console.log("Start position missing on active route, auto-locating...");
            if (typeof mainWindow !== 'undefined' && mainWindow !== null) {
                mainWindow.fetchDeviceLocation(mapContainer);
            }
        }
        
        updateRoute();
    }
    function updateFromBridge(lat, lon) {
        map.center = QtPositioning.coordinate(lat, lon);
        startMarker.coordinate = QtPositioning.coordinate(lat, lon);
        mapContainer.startLat = lat;
        mapContainer.startLon = lon;
        if (!mapContainer.isInitialSet) {
             marker.coordinate = QtPositioning.coordinate(lat, lon);
             mapContainer.currentLat = lat;
             mapContainer.currentLon = lon;
             mapContainer.currentAddress = lat.toFixed(5) + ", " + lon.toFixed(5);
        }
        updateRoute();
    }

    function updatePhoneLocation(lat, lon) {
        phoneMarker.coordinate = QtPositioning.coordinate(lat, lon);
        phoneMarker.visible = true;
        map.center = phoneMarker.coordinate;
    }
}
