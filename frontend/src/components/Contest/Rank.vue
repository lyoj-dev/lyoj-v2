<script setup lang="ts">
const props = defineProps([ 'data' ]);
</script>

<template>
    <v-card class="d-flex ContestCard card-radius">
        <p style="width: 10%">{{ data[0] }}</p>
        <p style="width: 20%" class="ellipsis" v-if="data[1].length == 1">{{ data[1][0] }}</p>
        <router-link style="width: 20%" class="ellipsis" v-if="data[1].length == 2" :to="data[1][1]">{{ data[1][0] }}</router-link>
        <span 
            v-for="item in data[2]"
            :style="'width: ' + (70 / data[2].length) + '%'"
            class="ellipsis d-flex justify-center"
        >
            <router-link
                :to="item[1]"
                v-if="item.length == 2"
                v-html="item[0]"
            ></router-link>
            <p
                v-if="item.length == 1"
                v-html="item[0]"
            ></p>
        </span>
    </v-card>
</template>

<style lang="css" scoped>
.ContestCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}
</style>