class WarrantTypeSelector extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    data2: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.handleAction = this.handleAction.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {}
  
  handleAction(event) {
    var action = event.target.name
    if (action=='normal') var type = 1
    else if (action=='winPrice') var type = 2
    
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var issuer = states.issuer.curIssuer
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    var command = {cmd: 'wselecttype', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), issuer: issuer, type: type}
    sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {normal: 'Normal', winPrice: 'Win Price'},
      sc: {normal: '正常', winPrice: 'Win Price'},
      tc: {normal: '正常', winPrice: 'Win Price'},
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    
    var curIssuer = this.props.data.curIssuer
    
    var btn1Style = 'btn-secondary', btn1IsDisable = true
    var btn2Style = 'btn-secondary', btn2IsDisable = true
    
    if (('curIssuer' in this.props.data) && (this.props.data.curIssuer in this.props.data2)) {
      var {normal, winPrice} = this.props.data2[this.props.data.curIssuer]
      // 預設
      if (!normal && !winPrice) {
        btn1Style = 'btn-secondary', btn1IsDisable = false
        btn2Style = 'btn-secondary', btn2IsDisable = true
      }
      // 選項1
      else if (normal) {
        btn1Style = 'btn-success', btn1IsDisable = true
        btn2Style = 'btn-secondary', btn2IsDisable = false
      }
      // 選項2
      else if (winPrice) {
        btn1Style = 'btn-secondary', btn1IsDisable = false
        btn2Style = 'btn-primary', btn2IsDisable = true
      }
    }
    
    return(
      <React.Fragment>
      
      <button
        name="normal"
        type="button"
        className={classNames('btn btn-sm', btn1Style)}
        disabled={btn1IsDisable}
        onClick={this.handleAction}>
          {text.normal}
      </button>
      
      <button
        name="winPrice"
        type="button"
        className={classNames('btn btn-sm', btn2Style)}
        disabled={btn2IsDisable}
        onClick={this.handleAction}>
          {text.winPrice}
      </button>
      
      </React.Fragment>
    )
  }
}
