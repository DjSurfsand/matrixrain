import QtQuick

// Authentic falling-code: each column keeps its own head position, speed,
// trail length and row of glyphs. Only the head and a short fading tail are
// painted each tick, so a full screen stays cheap.
Canvas {
    id: rain
    anchors.fill: parent
    renderTarget: Canvas.FramebufferObject
    contextType: "2d"

    // Density 1..10 maps to a cell of 23..14 pixels: more density, finer code.
    property int cell: 24 - backend.density
    // A near-white head and a pale second glyph keep the classic bright-tail
    // look while still following whatever accent the current theme sets.
    property string headColor: mixWithWhite(theme.accent, 0.82)
    property string secondColor: mixWithWhite(theme.accent, 0.4)
    property int columns: 0

    function mixWithWhite(hex, amount) {
        var r = parseInt(hex.slice(1, 3), 16)
        var g = parseInt(hex.slice(3, 5), 16)
        var b = parseInt(hex.slice(5, 7), 16)
        r = Math.round(r + (255 - r) * amount)
        g = Math.round(g + (255 - g) * amount)
        b = Math.round(b + (255 - b) * amount)
        return "rgb(" + r + "," + g + "," + b + ")"
    }
    property var heads: []
    property var speeds: []
    property var trails: []
    property var glyphRows: []

    function glyph() {
        var s = backend.glyphs
        return s.charAt(Math.floor(Math.random() * s.length))
    }

    function build() {
        if (width <= 0 || height <= 0)
            return
        var n = Math.max(8, Math.min(240, Math.floor(width / cell)))
        var h = [], sp = [], tr = [], rows = []
        for (var i = 0; i < n; i++) {
            // Start heads across the whole screen so the wallpaper is alive
            // immediately, rather than waiting for half the columns to fall in.
            h.push(Math.random() * height)
            sp.push(0.55 + Math.random() * 1.1)
            tr.push(12 + Math.floor(Math.random() * 18))
            var row = []
            for (var j = 0; j < 32; j++)
                row.push(glyph())
            rows.push(row)
        }
        heads = h
        speeds = sp
        trails = tr
        glyphRows = rows
        columns = n
        requestPaint()
    }

    Component.onCompleted: build()
    onWidthChanged: build()
    onHeightChanged: build()
    onCellChanged: build()

    function advance() {
        if (!columns || width <= 0 || height <= 0)
            return
        var factor = 0.45 + backend.speed * 0.18
        for (var i = 0; i < columns; i++) {
            var y = heads[i] + cell * speeds[i] * factor
            if (y - trails[i] * cell > height) {
                y = -Math.random() * height * 0.25
                speeds[i] = 0.55 + Math.random() * 1.1
                trails[i] = 12 + Math.floor(Math.random() * 18)
            }
            heads[i] = y
            var row = glyphRows[i]
            // A few glyphs shimmer every tick, like the original's flicker.
            for (var k = 0; k < 3; k++)
                if (Math.random() < 0.25)
                    row[Math.floor(Math.random() * row.length)] = glyph()
        }
        requestPaint()
    }

    Timer {
        interval: 33
        running: rain.visible && backend.enabled
        repeat: true
        onTriggered: rain.advance()
    }

    onPaint: {
        var ctx = getContext("2d")
        if (!ctx || !columns)
            return
        ctx.clearRect(0, 0, width, height)
        ctx.font = "bold " + cell + "px monospace"
        ctx.textBaseline = "top"
        var accent = theme.accent
        for (var i = 0; i < columns; i++) {
            var x = i * cell
            var row = glyphRows[i]
            var trail = trails[i]
            var top = heads[i]
            for (var t = trail - 1; t >= 0; t--) {
                var y = top - t * cell
                if (y < -cell || y > height)
                    continue
                if (t === 0) {
                    ctx.fillStyle = headColor
                    ctx.globalAlpha = 1
                } else if (t === 1) {
                    ctx.fillStyle = secondColor
                    ctx.globalAlpha = 0.95
                } else {
                    ctx.fillStyle = accent
                    ctx.globalAlpha = Math.max(0.08, 1 - t / trail) * 0.95
                }
                ctx.fillText(row[(i * 7 + t) % row.length], x, y)
            }
        }
        ctx.globalAlpha = 1
    }
}
