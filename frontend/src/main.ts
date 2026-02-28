import './assets/main.css'
import './assets/hljs.css'
import './assets/code.css'
import './assets/markdown.css'

import { createApp } from 'vue'
import App from './App.vue'
import { router } from './router'

// @mdi/font
import '@mdi/font/css/materialdesignicons.css'

// nprogress
import NProgress from 'nprogress'
import 'nprogress/nprogress.css'
NProgress.configure({
    parent: ".nProcessBar"
})

// vuetify
import 'vuetify/styles/main.css'
import { createVuetify } from 'vuetify'
import * as components from 'vuetify/components'
// import { VTimePicker } from 'vuetify/labs/components'

// vue-i18n
import { i18n } from './i18n'

// monaco-editor
import editorWorker from 'monaco-editor/esm/vs/editor/editor.worker?worker';
import jsonWorker from 'monaco-editor/esm/vs/language/json/json.worker?worker';
import cssWorker from 'monaco-editor/esm/vs/language/css/css.worker?worker';
import htmlWorker from 'monaco-editor/esm/vs/language/html/html.worker?worker';
import tsWorker from 'monaco-editor/esm/vs/language/typescript/ts.worker?worker';
(self as any).MonacoEnvironment = {
    getWorker(_: any, label: any) {
        if (label === 'json') {
            return new jsonWorker();
        }
        if (label === 'css' || label === 'scss' || label === 'less') {
            return new cssWorker();
        }
        if (label === 'html' || label === 'handlebars' || label === 'razor') {
            return new htmlWorker();
        }
        if (label === 'typescript' || label === 'javascript') {
            return new tsWorker();
        }
        return new editorWorker();
    },
};

const vuetify = createVuetify({ components: { ...components }, theme: { defaultTheme: 'system' } })
const app = createApp(App)
app.use(router).use(i18n).use(vuetify).mount('#app')
