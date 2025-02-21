<script setup lang="ts">
import { ref, watch, watchEffect } from 'vue';
import { appBarConfig } from './config';
import { i18n, message, type messageType } from '@/i18n';
import { useRoute } from 'vue-router';
import AppBarButton from './AppBarButton.vue';
import { storage } from '@/settings';
import { config } from '@/config.ts';
import { loginAs, loginInfo } from '@/utils.ts';
import { goto } from '@/router';

const t = i18n.global.t;
const route = useRoute();
const title = ref(config.title.short);
const locale = ref(storage.value.locale);

var items: string[] = [];
for (var key in message) items.push(key);

watch(() => route.name, () => {
	title.value = config.title.short;
	document.title = route.name ? t('route.' + (route.name as string)) + ' - ' + config.title.full : config.title.full;
});
watch(locale, () => {
    i18n.global.locale.value = locale.value as messageType;
    i18n.global.setLocaleMessage(locale.value, message[locale.value as messageType]);
});

function setLocale(val: string) {
    locale.value = val;
    storage.value.locale = val;
}
</script>

<template>
    <div class="d-flex justify-space-around elevation-7 AppBar">
        <div class="d-flex AppBar-left">
            <div class="d-flex align-center" style="margin-right: 10px;" @click="goto('index', {})">
                <img src="/favicon.png" class="AppBar-left-favicon"/>
                <p class="AppBar-left-title">&nbsp;{{ title }}</p>
            </div>

            <AppBarButton 
                v-for="item in appBarConfig.left"
                :title="t(item.title)"
                :icon="item.icon"
                :href="item.href"
                :prefix="item.prefix"
            ></AppBarButton>

            <AppBarButton
                v-if="loginAs != 0 && (loginInfo.permission & (1 << 10)) != 0"
                :title="t('appbar.admin')"
                icon="mdi-cog"
                href="/admin/index"
                prefix="/admin"
            ></AppBarButton>
        </div>
        <div class="d-flex AppBar-right">
            <AppBarButton 
                v-for="item in appBarConfig.right"
                :title="t(item.title)"
                :icon="item.icon"
                :href="item.href"
                :prefix="item.prefix"
            ></AppBarButton>

            <AppBarButton
                v-if="loginAs == 0"
                :title="t('appbar.login')"
                icon="mdi-arrow-right-circle"
                href="/login"
                prefix="/login"
            ></AppBarButton>
            <AppBarButton
                v-if="loginAs != 0"
                :title="loginInfo.title"
                icon=""
                :href="'/users/' + loginAs"
                :prefix="'/users/' + loginAs"
            ></AppBarButton>
            <AppBarButton
                v-if="loginAs != 0"
                :title="t('appbar.logout')"
                icon="mdi-power"
                href="/logout"
                prefix="/logout"
            ></AppBarButton>

            <div class="d-flex align-center AppBar-language">
                <v-icon icon="mdi-earth" size="small" style="margin-top: 3px;"></v-icon>
                <p>&nbsp;</p>
                <v-select
                    density="compact"
                    variant="outlined"
                    hide-details
                    :items="items"
                    :model-value="locale"
                    @update:model-value="(val) => setLocale(val)"
                ></v-select>
            </div>
        </div>
    </div>
</template>

<style lang="css" scoped>
.AppBar {
    min-height: 53px; 
    background-color: var(--color-background);
    position: fixed;
    top: 0px;
    width: 100%;
    z-index: 1030;
}

.AppBar-left-favicon {
    width: 32px;
    height: 32px;
    cursor: pointer;
}

.AppBar-left-title {
    font-size: 20px;
    line-height: 32px;
    font-family: 'Exo 2';
    cursor: pointer;
}

.AppBar-language {
    color: var(--color-text)!important;
    background: rgba(0, 0, 0, 0)!important;
    padding: 0px 10px;
    cursor: pointer;
    border: 3px solid;
    border-left: 0px solid;
    border-right: 0px solid;
    border-color: rgba(0, 0, 0, 0);
    transition: border-top-color 0.3s;
    margin-left: 4px;
}
.AppBar-language:hover {
    border-top-color: orange;
}
</style>

<style lang="css">
.AppBar .v-input__control {
    height: 30px;
}
.v-overlay-container .v-field__field,
.AppBar .v-field__field {
    font-size: 14px;
    height: 30px;
}
.v-overlay-container .v-field__input,
.AppBar .v-field__input {
    min-height: 30px!important;
    padding-top: 0px!important;
    padding-bottom: 0px!important;
}
.v-overlay-container .v-selection-control__input, .v-overlay-container .v-selection-control__wrapper,
.AppBar .v-selection-control__input, .AppBar .v-selection-control__wrapper {
    width: 32px!important;
    height: 24px!important;
}
.v-overlay-container .v-list-item,
.AppBar .v-list-item {
    min-height: 20px!important;
}
</style>