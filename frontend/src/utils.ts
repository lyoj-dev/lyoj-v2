import { ref } from "vue";

export function sleep(ms: number) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

export function setCookie(key: string, value: string, time: number) {
    const expires = new Date(Date.now() + time * 1000).toUTCString();
    document.cookie = `${key}=${value}; expires=${expires}; path=/`;
}

export function getCookie() {
    const cookies = document.cookie.split(';');
    if (document.cookie === '') return {};
    const cookieMap: any = {};
    for (const cookie of cookies) {
        const [key, value] = cookie.split('=');
        cookieMap[key.trim()] = value.trim();
    }
    return cookieMap;
}

export function deleteCookie(key: string) {
    const expires = new Date(0).toUTCString();
    console.log(`${key}=; expires=${expires}`);
    document.cookie = `${key}=; expires=${expires}; path=/`;
}

var errorListener = 0;
var errorListener2 = 0;
export var isError = ref(false);
export var type = ref("success");
export var errorText = ref("");
export function showMsg(errorType: string, msg: string) {
	if (errorListener != 0) { clearTimeout(errorListener); }
	if (errorListener2 != 0) { clearTimeout(errorListener2); }
	isError.value = false;
	errorListener = setTimeout(() => { 
		isError.value = true;
		type.value = errorType;
		errorText.value = msg;
	}, 500);
	errorListener2 = setTimeout(() => { isError.value = false; }, 5000);
}

export const loginAs = ref(0);
export const loginInfo: any = ref({});
export async function myFetch(url: string, options?: RequestInit) {
    if (options == undefined) options = {};
    if (options.headers == undefined) options.headers = {};
    (options.headers as any)['Authorization'] = 'SessionToken ' + getCookie().session;
    var response = await fetch(url, options);
    if (response.status != 200) {
        showMsg('error', response.statusText);
        throw new Error(response.statusText);
    }
    var data = await response.json();
    if (data.code != 200) {
        showMsg('error', data.code + " " + data.msg);
        throw new Error(data.code + " " + data.msg);
    }
    loginAs.value = data.loginAs;
    loginInfo.value = data.loginInfo;
    return data;
}