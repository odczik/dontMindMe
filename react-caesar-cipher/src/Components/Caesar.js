import {React, useState, useEffect, useCallback} from "react"
import {cipher} from "./cipher"

function Caesar() {
    const [caesar, setCaesar] = useState(3)
    const [live, setLive] = useState(false)
    const [encryption, setEncryption] = useState(true)
    const [text, setText] = useState("")
    const [processedText, setProcessedText] = useState("")

    const process = useCallback(() => {
        //let chArray = []
        //text.split("").forEach(ch => {
        //    encryption ? chArray.push(String.fromCharCode(ch.charCodeAt(0) + parseFloat(caesar))) : chArray.push(String.fromCharCode(ch.charCodeAt(0) - parseFloat(caesar)))
        //})
        //setProcessedText(chArray.join(""))
        setProcessedText(cipher(text, caesar, encryption))
    }, [text, caesar, encryption])

    const copyToClipboard = (text) => {
        if(!navigator.clipboard) return alert("error")
        navigator.clipboard.writeText(text)
    }

    useEffect(() => {
        if(!live) return
        process()
    }, [text, caesar, encryption, live, process])

    //const alphabet = ["A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z"]

    return (
        <div>
            <label>
                Text:&nbsp;
                <input type="text" onChange={e => setText(e.target.value)} />
            </label>
            <br />
            <label>
                Caesar:&nbsp;
                <input type="range" defaultValue="3" min="1" max="100" onChange={e => setCaesar(e.target.value)} />
                {caesar}
            </label>
            <br />
            <label>
                Live:&nbsp;
                <input type="checkbox" value={live} onChange={() => setLive(!live)} />
            </label>
            <br />
            <label>
                Encrypt <input type="radio" value="Encrypt" onChange={() => setEncryption(true)} checked={encryption} />
                <br />
                Decrypt <input type="radio" value="Decrypt" onChange={() => setEncryption(false)} checked={!encryption} />
            </label>
            <br />
            <button onClick={() => process()} disabled={live}>Process</button>
            <br />
            <label>
                <textarea value={processedText} readOnly />
            </label>
            <br />
            <button onClick={() => copyToClipboard(processedText)}>Copy to clipboard</button>
        </div>
    );
}

export default Caesar;