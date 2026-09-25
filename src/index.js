import Reveal from 'reveal.js'
import Markdown from 'reveal.js/plugin/markdown'
import Highlight from 'reveal.js/plugin/highlight'
import Math from 'reveal.js/plugin/math'
import Notes from 'reveal.js/plugin/notes'

import 'reveal.js/reset.css'
import 'reveal.js/reveal.css'
import 'reveal.js/theme/night.css'
import 'reveal.js/plugin/highlight/monokai.css'

let deck = new Reveal({
   plugins: [ Markdown, Highlight, Notes, Math ],
   slideNumber: 'c/t',
   hash: true,
   pdfSeparateFragments: false,
   width: 1440,
   height: 900,
   margin: 0.08
})

deck.initialize()