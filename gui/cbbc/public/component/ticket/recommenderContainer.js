class RecommenderContainer extends React.Component {
  constructor(props) {
    super(props)
    this.setStates = this.setStates.bind(this)
    this.getStates = this.getStates.bind(this)
    
    this.state = {}
    this.state.issuerList = getIssuer()
    this.state.userId = null
    this.state.cellsConfig = null
    this.state.cells = null
    this.state.modules = null
    this.state.lastGetDataTime = null
  }
  
  componentDidMount() {
    var that = this
    window.addEventListener("message", (event) => {
      if (('action' in event.data) && event.data.action == 'send_msg') {
        that.setState({userId: event.data.userId})
        that.setState({cellsConfig: event.data.cellsConfig})
        that.setState({cells: event.data.cells})
        
        var modules = {}
        for (var v of event.data.modules) {
          var v1 = v.split(',')
          if (v1.length>1)
            modules[v1[0]] = v1[1]
        }
        that.setState({modules: modules})
        that.setState({lastGetDataTime: moment()})
      }
    })
    
    //
    setInterval(function(){
      var diff = moment().diff(that.state.lastGetDataTime, 'seconds')
      if (diff > 10) {
        close()
      }
    }, 3000);
  }
  
  setStates(states) {this.setState(states)}
  
  getStates() {return this.state}
  
  render() {
    return(
      <React.Fragment>
      <Recommender
        key="recommender"
        data={this.state.issuerList}
        data2={this.state.cellsConfig}
        data3="HSI"
        lang={this.props.lang}
        setStates={this.setStates}
        getStates={this.getStates}
      />
      </React.Fragment>
    )
  }
}

ReactDOM.render(
  <RecommenderContainer 
    lang={global.cookies['cbbc-lang']}
  />, 
document.getElementById('cbbc'))