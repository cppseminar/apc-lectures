import Reveal from 'reveal.js'
import Markdown from 'reveal.js/plugin/markdown/markdown.esm.js'
import Highlight from 'reveal.js/plugin/highlight/highlight.esm.js'
import Notes from 'reveal.js/plugin/notes/notes.esm.js'

import 'reveal.js/dist/reset.css'
import 'reveal.js/dist/reveal.css'
import 'reveal.js/dist/theme/night.css'
import 'reveal.js/plugin/highlight/zenburn.css'

let deck = new Reveal({
   plugins: [ Markdown, Highlight, Notes ],
   slideNumber: 'c/t',
   hash: true,
   pdfSeparateFragments: false,
   width: 1440,
   height: 900,
})

deck.initialize()