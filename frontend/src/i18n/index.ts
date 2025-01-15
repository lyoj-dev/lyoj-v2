import { zh_cn } from "./zh_cn.ts"
import { en_us } from './en_us.ts'
import { createI18n } from 'vue-i18n'
import { storage } from "@/settings/index.ts"

export const message = { 
    "zh_cn": zh_cn, 
    "en_us": en_us
};
export type messageType = keyof typeof message;
export const i18n = createI18n({
    legacy: false,
    locale: storage.value.locale,
    messages: message
})
