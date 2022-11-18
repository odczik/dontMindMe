export const cipher = (text, value, decrypt) => {
    let chArray = []
    text.split("").forEach(ch => {
        decrypt ? chArray.push(String.fromCharCode(ch.charCodeAt(0) - parseFloat(value))) : chArray.push(String.fromCharCode(ch.charCodeAt(0) + parseFloat(value)))
    })
    return chArray.join("")
}