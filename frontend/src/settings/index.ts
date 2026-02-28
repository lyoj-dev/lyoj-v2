import { useStorage } from "@vueuse/core";

export const storage = useStorage("Settings", {
    locale: "zh_cn",
});