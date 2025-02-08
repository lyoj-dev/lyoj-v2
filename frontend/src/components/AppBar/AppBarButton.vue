<script setup lang="ts">
import { goto } from '@/router';
import { computed } from 'vue';
import { useRoute } from 'vue-router';

const props = defineProps([ 'title', 'icon', 'href' ]);
const route = useRoute();

const active = computed(() => {
    const href = props.href;
    return !!href && (route.path === href || route.path.startsWith(`${href}/`));
});
</script>

<template>
    <router-link
        :class="'d-flex align-center AppBarButton' + (active ? ' AppBarButton-active' : '')"
        :to="href"
    >
        <v-icon 
            v-if="icon != ''"
            :icon="icon" 
            size="small" 
            style="margin-top: 3px;"
        ></v-icon>
        <p class="AppBarButton-title">&nbsp;{{ title }}</p>
    </router-link>
</template>

<style lang="css" scoped>
.AppBarButton {
    color: var(--color-text)!important;
    background: rgba(0, 0, 0, 0)!important;
    padding: 0px 10px;
    cursor: pointer;
    border: 3px solid;
    border-left: 0px solid;
    border-right: 0px solid;
    border-color: rgba(0, 0, 0, 0);
    transition: border-top-color 0.3s;
    margin: 0px 4px;
}
.AppBarButton:hover {
    border-top-color: orange;
}

.AppBarButton-active {
    border-top-color: orange!important;
}

.AppBarButton-title {
    font-size: 15px;
    font-family: 'Exo 2';
    line-height: 32px;
}
</style>