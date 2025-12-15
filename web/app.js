document.addEventListener('DOMContentLoaded', () => {
    const analyzeBtn = document.getElementById('analyzeBtn');
    const sourceCode = document.getElementById('sourceCode');
    const resultsSection = document.getElementById('resultsSection');
    const tabs = document.querySelectorAll('.tab-btn');
    const contents = document.querySelectorAll('.tab-content');
    const sampleButtons = document.querySelectorAll('.sample-btn');

    const samples = {
        valid: `int x = 10;
int y = 20;
if (x < y) {
    int z = x + y;
    x = z * 2;
}
while (x > 0) {
    x = x - 1;
}`,
        syntax: `int x = 10
if (x > 5) {
    x = x + 1;
}
/* Missing semicolon on line 1 */`,
        semantic: `int x = 10;
if (x > 5) {
    int y = x + 1;
}
int z = y; 
/* Error: y is defined in the if-block scope 
   and cannot be accessed here */`,
        lexical: `int x = 10;
int $invalid = 20; 
/* Error: '$' is not a valid character 
   for identifiers in this language */`,
        comments: `// Single-line comment example
int a = 5; // Inline comment

/* 
   Multi-line comment block
   Variables below should be parsed correctly
*/
int b = 10;

// simple calculation
int c = a + b;`
    };

    sampleButtons.forEach(btn => {
        btn.addEventListener('click', () => {
            const key = btn.dataset.key;
            if (samples[key]) {
                sourceCode.value = samples[key];
            }
        });
    });

    // Tab Switching Logic
    tabs.forEach(tab => {
        tab.addEventListener('click', () => {
            tabs.forEach(t => t.classList.remove('active'));
            contents.forEach(c => c.classList.remove('active'));

            tab.classList.add('active');
            document.getElementById(tab.dataset.tab).classList.add('active');
        });
    });

    // Analyze Button Logic
    analyzeBtn.addEventListener('click', async () => {
        const code = sourceCode.value;
        if (!code.trim()) return;

        analyzeBtn.disabled = true;
        analyzeBtn.textContent = "Analyzing...";

        try {
            const response = await fetch('/api/analyze', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ source: code })
            });

            if (!response.ok) {
                throw new Error('Analysis failed');
            }

            const data = await response.json();
            renderResults(data);
            resultsSection.style.display = 'block';
        } catch (error) {
            alert("Error communicating with backend: " + error.message);
        } finally {
            analyzeBtn.disabled = false;
            analyzeBtn.textContent = "Analyze Code";
        }
    });

    function renderResults(data) {
        // Tokens
        const tokensBody = document.getElementById('tokensTableWithReq');
        tokensBody.innerHTML = data.tokens.map(t => `
            <tr>
                <td>${t.line}</td>
                <td><span style="color: #4ec9b0">${t.type}</span></td>
                <td>${escapeHtml(t.value)}</td>
            </tr>
        `).join('');

        // Symbol Table
        const symbolBody = document.getElementById('symbolTableBody');
        symbolBody.innerHTML = data.symbolTable.map(s => `
            <tr>
                <td>${s.line}</td>
                <td>${s.name}</td>
                <td><span style="color: #569cd6">${s.type}</span></td>
            </tr>
        `).join('');

        // Syntax Errors
        const syntaxList = document.getElementById('syntaxErrorsList');
        if (data.syntaxErrors.length === 0) {
            syntaxList.innerHTML = '<li style="border-left-color: #4ec9b0; color: #4ec9b0">No Syntax Errors Found</li>';
        } else {
            syntaxList.innerHTML = data.syntaxErrors.map(e => `<li>${e}</li>`).join('');
        }

        // Semantic Errors
        const semanticList = document.getElementById('semanticErrorsList');
        if (data.semanticErrors.length === 0) {
            semanticList.innerHTML = '<li style="border-left-color: #4ec9b0; color: #4ec9b0">No Semantic Errors Found</li>';
        } else {
            semanticList.innerHTML = data.semanticErrors.map(e => `<li>${e}</li>`).join('');
        }

        // Update tab counts if desired (optional)
        document.querySelector('[data-tab="syntaxErrors"]').textContent = `Syntax Errors (${data.syntaxErrors.length})`;
        document.querySelector('[data-tab="semanticErrors"]').textContent = `Semantic Errors (${data.semanticErrors.length})`;
    }

    function escapeHtml(text) {
        const map = {
            '&': '&amp;',
            '<': '&lt;',
            '>': '&gt;',
            '"': '&quot;',
            "'": '&#039;'
        };
        return text.replace(/[&<>"']/g, function (m) { return map[m]; });
    }
});
